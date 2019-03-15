#include "liver.h"
#include "serialize_buf.h"

extern "C" {
#include "librtmp/rtmp.h"
#include "flv-mux.h"
#include "rtmp-helpers.h"
}

#include <QDebug>
#include <array>

Liver::Liver(QObject *parent) : QObject(parent)
{

}

Liver::~Liver()
{

}

bool Liver::init(const uint8_t *video_header_data, size_t video_header_sz, MetaData && metadata)
{
    _video_header.assign(video_header_data, video_header_data + video_header_sz);

    WORD version;
    WSADATA wsaData;

    version = MAKEWORD(2, 2);

    int startup_ret = WSAStartup(version, &wsaData);
    qDebug() << "WSAStartup return " << startup_ret;

    _rtmp = RTMP_Alloc();

    RTMP_Init(_rtmp);
    _rtmp->Link.timeout=5;

    int ret = RTMP_SetupURL(_rtmp, const_cast<char *>(_stream_base_url.c_str()));
    if(!ret)
    {
        qDebug() << "RTMP_SetupURL return: " << ret << " " << _stream_base_url.c_str();
        return false;
    }

    RTMP_EnableWrite(_rtmp);

    qDebug() << "Liver init 1";

    RTMP_AddStream(_rtmp, _stream_name.c_str());

    ret = RTMP_Connect(_rtmp, NULL);
    if(!ret)
    {
        qDebug() << "RTMP_Connect return: " << ret << " " << _stream_base_url.c_str();
        return false;
    }

    qDebug() << "Liver init 2";

    ret = RTMP_ConnectStream(_rtmp, 0);
    if(!ret)
    {
        qDebug() << "RTMP_ConnectStream return: " << ret;
        return false;
    }

    _metadata = metadata;

    // send_meta_data();

    qDebug() << "Liver init over";

    return true;
}

void Liver::streamUrlGot(const QString &url)
{
    qDebug() << "streamUrlGot: " << url;

    std::string whole_url = url.toStdString();

    if(whole_url.empty())
    {
        return;
    }

    if(whole_url.back() == '/')
    {
        whole_url.pop_back();
    }

    auto last_slash_it = whole_url.find_last_of('/');

    if(last_slash_it == std::string::npos)
    {
        return;
    }

    _stream_base_url = whole_url.substr(0, last_slash_it);
    _stream_name = whole_url.substr(last_slash_it + 1);

    qDebug() << _stream_base_url.c_str() << ":" << _stream_name.c_str();
}

//void Liver::flv_packet_mux(struct encoder_packet *packet, int32_t dts_offset,
//                           std::vector<uint8_t> &output_buf, bool is_header)
//{
//    flv_video(packet, dts_offset, output_buf, is_header);
//}

//void Liver::flv_video(struct encoder_packet *packet, int32_t dts_offset,
//                    std::vector<uint8_t> &output_buf, bool is_header)
//{

//}

void Liver::on_stream_data(encoder_packet &&pkt)
{
    if(!_send_headers)
    {
        send_header();
    }

    uint8_t *buf = nullptr;
    size_t buf_size = 0;

    flv_packet_mux(&pkt, 0, &buf, &buf_size, false);

    int ret = RTMP_Write(_rtmp, (char*)buf, (int)buf_size, 0);
    if(ret <= 0)
    {
        qDebug() << "RTMP_Write return: " << ret;
    }
}

static inline bool has_start_code(const uint8_t *data)
{
    if(data[0] != 0 || data[1] != 0)
        return false;

    return data[2] == 1 || (data[2] == 0 && data[3] == 1);
}

/* NOTE: I noticed that FFmpeg does some unusual special handling of certain
 * scenarios that I was unaware of, so instead of just searching for {0, 0, 1}
 * we'll just use the code from FFmpeg - http://www.ffmpeg.org/ */
inline const uint8_t *ff_avc_find_startcode_internal(const uint8_t *p,
        const uint8_t *end)
{
    const uint8_t *a = p + 4 - ((intptr_t)p & 3);

    for (end -= 3; p < a && p < end; p++) {
        if (p[0] == 0 && p[1] == 0 && p[2] == 1)
            return p;
    }

    for (end -= 3; p < end; p += 4) {
        uint32_t x = *(const uint32_t*)p;

        if ((x - 0x01010101) & (~x) & 0x80808080) {
            if (p[1] == 0) {
                if (p[0] == 0 && p[2] == 1)
                    return p;
                if (p[2] == 0 && p[3] == 1)
                    return p+1;
            }

            if (p[3] == 0) {
                if (p[2] == 0 && p[4] == 1)
                    return p+2;
                if (p[4] == 0 && p[5] == 1)
                    return p+3;
            }
        }
    }

    for (end += 3; p < end; p++) {
        if (p[0] == 0 && p[1] == 0 && p[2] == 1)
            return p;
    }

    return end + 3;
}

inline const uint8_t * obs_avc_find_startcode(const uint8_t *p, const uint8_t *end)
{
    const uint8_t *out= ff_avc_find_startcode_internal(p, end);
    if (p < out && out < end && !out[-1]) out--;
    return out;
}

inline void get_sps_pps(const uint8_t *data, size_t size,
                        const uint8_t **sps, size_t *sps_size,
                        const uint8_t **pps, size_t *pps_size)
{
    const uint8_t *nal_start, *nal_end;
    const uint8_t *end = data + size;
    int type;

    nal_start = obs_avc_find_startcode(data, end);
    while(true)
    {
        while(nal_start < end && !*(nal_start++));

        if(nal_start == end)
            break;

        nal_end = obs_avc_find_startcode(nal_start, end);

        type = nal_start[0] & 0x1F;
        if(type == OBS_NAL_SPS)
        {
            *sps = nal_start;
            *sps_size = nal_end - nal_start;
        }
        else if(type == OBS_NAL_PPS)
        {
            *pps = nal_start;
            *pps_size = nal_end - nal_start;
        }

        nal_start = nal_end;
    }
}

void Liver::send_header()
{
    uint8_t *buf = nullptr;
    size_t buf_size = 0;

    encoder_packet pkt;

    qDebug() << "has start code: " << has_start_code(_video_header.data());

    const uint8_t *sps = NULL, *pps = NULL;
    size_t sps_size = 0, pps_size = 0;

    get_sps_pps(_video_header.data(), _video_header.size(), &sps, &sps_size, &pps, &pps_size);
    if(!sps || !pps || sps_size < 4)
    {
        qDebug() << "get_sps_pps failed";
        return;
    }

    serialize_buf header;

    header.write_8b(0x01)
    .write(sps + 1, 3)
    .write_8b(0xff)
    .write_8b(0xe1);

    header.write_16b((uint16_t)sps_size)
    .write(sps, sps_size);

    header.write_8b(0x01)
    .write_16b((uint16_t)pps_size)
    .write(pps, pps_size);

    pkt.data = header.data();
    pkt.size = header.size();
    pkt.keyframe = true;
    pkt.timebase_den = 1;
    pkt.type = OBS_ENCODER_VIDEO;

    flv_packet_mux(&pkt, 0, &buf, &buf_size, true);

    qDebug() << "out size: " << buf_size;

    int ret = RTMP_Write(_rtmp, (char*)buf, (int)buf_size, 0);
    if(ret <= 0)
    {
        qDebug() << "header RTMP_Write return: " << ret;
    }

    _send_headers = true;
}

void Liver::send_meta_data()
{
    const size_t md_sz = 4096;
    std::array<char, md_sz> buf;

    char *enc = buf.data();
    char *end = enc + buf.size();

    enc_str(&enc, end, "onMetaData");

    *enc++ = AMF_ECMA_ARRAY;
    enc = AMF_EncodeInt32(enc, end, 14);

    enc_num_val(&enc, end, "duration", 0.0);
    enc_num_val(&enc, end, "fileSize", 0.0);

    enc_num_val(&enc, end, "width", (double)_metadata.width);
    enc_num_val(&enc, end, "height", (double)_metadata.height);

    enc_str_val(&enc, end, "videocodecid", "avc1");
    enc_num_val(&enc, end, "videodatarate", _metadata.video_bitrate);
    enc_num_val(&enc, end, "framerate", _metadata.fps);

//    enc_str_val(&enc, end, "audiocodecid", "mp4a");
//    enc_num_val(&enc, end, "audiodatarate", meta_data.audio_bitrate);
//    enc_num_val(&enc, end, "audiosamplerate",
//                (double)meta_data.samplerate);
//    enc_num_val(&enc, end, "audiosamplesize", 16.0);
//    enc_num_val(&enc, end, "audiochannels",
//                (double)meta_data.audio_channels);

//    enc_bool_val(&enc, end, "stereo",
//                 meta_data.audio_channels == 2);
//    enc_bool_val(&enc, end, "2.1",
//                 meta_data.audio_channels == 3);
//    enc_bool_val(&enc, end, "3.1",
//                 meta_data.audio_channels == 4);
//    enc_bool_val(&enc, end, "4.0",
//                 meta_data.audio_channels == 4);
//    enc_bool_val(&enc, end, "4.1",
//                 meta_data.audio_channels == 5);
//    enc_bool_val(&enc, end, "5.1",
//                 meta_data.audio_channels == 6);
//    enc_bool_val(&enc, end, "7.1",
//                 meta_data.audio_channels == 8);

    enc_str_val(&enc, end, "encoder", "encoder name");

    *enc++ = 0;
    *enc++ = 0;
    *enc++ = AMF_OBJECT_END;

    size_t sz = enc - buf.data();

    std::array<uint8_t, md_sz> pktbuf;

    auto p = pktbuf.data();
    auto startpos = p;
    *p++ = RTMP_PACKET_TYPE_INFO;

    p = write_24b(p, (uint32_t)sz);
    p = write_32b(p, 0);
    p = write_24b(p, 0);

    memcpy(p, buf.data(), sz);
    p += sz;

    p = write_32b(p, p - startpos - 1);

    int ret = RTMP_Write(_rtmp, (char*)pktbuf.data(), (int)(p - startpos), 0);

    qDebug() << "meta_data RTMP_Write return: " << ret;
}

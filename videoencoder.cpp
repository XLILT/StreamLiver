#pragma warning (disable:4819)

#include "videoencoder.h"

#include <QDebug>
#include <QFile>

extern "C" {
#include "libavutil/opt.h"
#include "libavutil/log.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavformat/avformat.h"
}

// QFile a_file("./a.h264");
AVOutputFormat *ofmt = NULL;
AVFormatContext *ofmt_ctx = NULL;
AVStream *out_stream = NULL;
const char * out_filename = "foo.flv";
// const char * out_filename = "foo1.mp4";
// const char * out_filename = "foo2.mkv";

VideoEncoder::VideoEncoder(QObject *parent) : QObject(parent)
{
    av_log_set_level(AV_LOG_DEBUG);

    _codec = avcodec_find_encoder_by_name("h264");
    _codectx = avcodec_alloc_context3(_codec);
    _pkt = av_packet_alloc();

    // a_file.open(QIODevice::ReadWrite);
}

VideoEncoder::~VideoEncoder()
{
    qDebug() << "destruct";

    //写文件尾
    if(ofmt_ctx)
    {
        av_write_trailer(ofmt_ctx);

        avcodec_free_context(&_codectx);

        if(_frame)
        {
            av_frame_free(&_frame);
        }

        if(_pkt)
        {
            av_packet_free(&_pkt);
        }

        if(_sws_context)
        {
            sws_freeContext(_sws_context);
        }

        //Close input
        if(!(ofmt->flags & AVFMT_NOFILE))
        {
            avio_closep(&(ofmt_ctx->pb));
        }

        avformat_free_context(ofmt_ctx);
    }

}

bool VideoEncoder::init(int w, int h, AVPixelFormat fmt, int fps)
{
    _raw_pix_fmt = fmt;

    // _codec = avcodec_find_encoder_by_name("h264");
    _codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if(!_codec)
    {
        qDebug() << "codec is null";
        return false;
    }

    _codectx = avcodec_alloc_context3(_codec);
    if(!_codectx)
    {
        qDebug() << "codeccontext is null";
        return false;
    }

//    auto p_supported_pix_fmt = _codec->pix_fmts;

//    while(p_supported_pix_fmt != nullptr && *p_supported_pix_fmt != AV_PIX_FMT_NONE)
//    {
//        qDebug() << "pix_fmts: " << *p_supported_pix_fmt;
//        ++p_supported_pix_fmt;
//    }



    /* put sample parameters */
    _codectx->bit_rate = 400000;
    /* resolution must be a multiple of two */
    _codectx->width = w;
    _codectx->height = h;
    /* frames per second */
    // _codectx->time_base = {1, 25};
    _codectx->time_base = {1, fps};
    // _codectx->framerate = {25, 1};
    _codectx->framerate = {fps, 1};

    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    _codectx->gop_size = 10;
    _codectx->max_b_frames = 1;
    _codectx->pix_fmt = AV_PIX_FMT_YUV420P;
    // _codectx->pix_fmt = AV_PIX_FMT_YUYV422;

    if (_codec->id == AV_CODEC_ID_H264)
    {
        av_opt_set(_codectx->priv_data, "preset", "slow", 0);
    }

    _pkt = av_packet_alloc();
    if(!_pkt)
    {
        qDebug() << "pkt is null";
        return false;
    }

    /* open it */
    int ret = avcodec_open2(_codectx, _codec, NULL);
    if (ret < 0)
    {
       char errbuf[1024] = {0};
       av_make_error_string(errbuf, sizeof errbuf, ret);

       qDebug() << "Could not open codec: " << errbuf;
       return false;
    }

    _frame = av_frame_alloc();
    if (!_frame)
    {
       qDebug() << "Could not allocate video frame";
       return false;
    }

    _frame->format = _codectx->pix_fmt;
    _frame->width  = _codectx->width;
    _frame->height = _codectx->height;

    // qDebug() << "bef " << _frame->data[0] << " " << _frame->linesize[0];

    // ret = av_frame_get_buffer(_frame, 32);
    ret = av_frame_get_buffer(_frame, 0);
    if (ret < 0)
    {
       char errbuf[1024] = {0};
       av_make_error_string(errbuf, sizeof errbuf, ret);

       qDebug() << "av_frame_get_buffer failed: " << errbuf;

       return false;
    }

    // qDebug() << "aft " << _frame->data[0] << " " << _frame->linesize[0];

    _sws_context = sws_getContext(_codectx->width, _codectx->height, _raw_pix_fmt,
                                  _codectx->width, _codectx->height, _codectx->pix_fmt,
                                  SWS_SINC, NULL, NULL, NULL);

    if(!_sws_context)
    {
       qDebug() << "sws_getContext return null";
       return false;
    }

    //打开输出流
    ret = avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);
    if (ret < 0)
    {
       char errbuf[1024] = {0};
       av_make_error_string(errbuf, sizeof errbuf, ret);

       qDebug() << "avformat_alloc_output_context2 failed: " << errbuf;

       return true;
    }

    out_stream = avformat_new_stream(ofmt_ctx, _codec);
    if(!out_stream)
    {
        qDebug() << "Failed allocating output stream";

        return true;
    }

    qDebug() << "nb_streams: " << ofmt_ctx->nb_streams;

    out_stream->id = ofmt_ctx->nb_streams - 1;
    out_stream->time_base = _codectx->time_base;

    AVCodecParameters avcp;

    ret = avcodec_parameters_from_context(&avcp, _codectx);
    if (ret < 0)
    {
       char errbuf[1024] = {0};
       av_make_error_string(errbuf, sizeof errbuf, ret);

       qDebug() << "avcodec_parameters_from_context failed: " << errbuf;

       return true;
    }

    ret = avcodec_parameters_to_context(out_stream->codec, &avcp);
    if (ret < 0)
    {
       char errbuf[1024] = {0};
       av_make_error_string(errbuf, sizeof errbuf, ret);

       qDebug() << "avcodec_parameters_to_context failed: " << errbuf;

       return true;
    }

    ofmt = ofmt_ctx->oformat;

    if(ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
    {
        out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    // av_dump_format(ofmt_ctx, 0, "foo.flv", 1);

    //打开输出文件
    if(!(ofmt->flags & AVFMT_NOFILE))
    {
        ret = avio_open(&ofmt_ctx->pb,out_filename,AVIO_FLAG_WRITE);
        if(ret<0)
        {
            qDebug() << "Could not open output URL " << out_filename;

            return true;
        }
    }

    //写文件头到输出文件
    ret = avformat_write_header(ofmt_ctx,NULL);
    if(ret < 0)
    {
        qDebug() << "Error occured when opening output URL";

        return true;
    }

   qDebug() << "encoder init over";

   return true;
}

void VideoEncoder::on_frame(uint8_t * data, int linesize, int64_t pts)
{
    // qDebug() << "on_frame " << _frame << " " << _codec << " " << _codectx << " " << _pkt;

    if(!_frame || !_codec || !_codectx || !_pkt)
    {
        return;
    }

    int scale_height = scale_frame(data, linesize);

    if(scale_height != _codectx->height)
    {
        qDebug() << "scale failed " << scale_height << " != " << _codectx->height;

        return;
    }

//    _frame->data[0] = data;
//    _frame->linesize[0] = linesize;
    _frame->pts = pts;

    int ret = avcodec_send_frame(_codectx, _frame);
    if (ret < 0)
    {
        qDebug() <<"Error sending a frame for encoding";
        return;
    }

    while (ret >= 0)
    {
        ret = avcodec_receive_packet(_codectx, _pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        {
            return;
        }
        else if (ret < 0)
        {
            qDebug() << "Error during encoding";
            return;
        }

        qDebug() << "1packet encoded (dts=" << _pkt->dts << ")(pts=" << _pkt->pts << ")(size=" << _pkt->size << ")";

        av_packet_rescale_ts(_pkt, _codectx->time_base, out_stream->time_base);

        qDebug() << "2packet encoded (dts=" << _pkt->dts << ")(pts=" << _pkt->pts << ")(size=" << _pkt->size << ")";

        // a_file.write((char *)_pkt->data, _pkt->size);

        //将包数据写入到文件。
        int write_ret = av_interleaved_write_frame(ofmt_ctx, _pkt);
        if(write_ret < 0)
        {
            /**
            当网络有问题时，容易出现到达包的先后不一致，pts时序混乱会导致
            av_interleaved_write_frame函数报 -22 错误。暂时先丢弃这些迟来的帧吧
            若所大部分包都没有pts时序，那就要看情况自己补上时序（比如较前一帧时序+1）再写入。
            */
//            if(ret == -22)
//            {
//                qDebug() << "av_interleaved_write_frame return 22";
//            }
//            else
//            {
//                printf("Error muxing packet.error code %d\n" , ret);
//                break;
//            }

            char errbuf[1024] = {0};
            av_make_error_string(errbuf, sizeof errbuf, write_ret);

            qDebug() << "av_interleaved_write_frame failed: " << errbuf;
        }

        av_packet_unref(_pkt);
    }
}

int VideoEncoder::scale_frame(uint8_t * in_data, int in_linesize)
{
    uint8_t * yuy2_slice[] = {in_data, nullptr, nullptr};
    int yuy2_stride[] = {_codectx->width * 2, 0, 0};

    int ret = sws_scale(_sws_context, yuy2_slice,
                  yuy2_stride, 0, _codectx->height,
                  _frame->data, _frame->linesize);

    // qDebug() << "sws_scale return: " << ret;

    (void)in_linesize;

    return ret;
}

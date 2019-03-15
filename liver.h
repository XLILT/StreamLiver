#ifndef LIVER_H
#define LIVER_H

#include "common_def.h"
#include "metadata_def.h"

#include <stdint.h>
#include <vector>
#include <string>
#include <QObject>
#include <QString>

struct RTMP;

class Liver : public QObject
{
    Q_OBJECT
public:
    explicit Liver(QObject *parent = nullptr);
    virtual ~Liver();

    bool init(const uint8_t *video_header_data, size_t video_header_sz, MetaData && metadata);

    void on_stream_data(encoder_packet &&pkt);

//    void flv_packet_mux(encoder_packet *packet, int32_t dts_offset,
//                        uint8_t **output, size_t *size, bool is_header);

//    void flv_video(encoder_packet *packet, int32_t dts_offset,
//                   uint8_t **output, size_t *size, bool is_header);

    void send_header();
    void send_meta_data();

signals:

public slots:
    void streamUrlGot(const QString &url);

private:
    RTMP * _rtmp = nullptr;

    bool _send_headers = false;
    std::vector<uint8_t> _video_header;

    std::string _stream_base_url;
    std::string _stream_name;

    MetaData _metadata;
};

#endif // LIVER_H

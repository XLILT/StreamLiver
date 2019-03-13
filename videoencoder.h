#ifndef VIDEOENCODER_H
#define VIDEOENCODER_H

#include <QObject>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
}

class VideoEncoder : public QObject
{
    Q_OBJECT
public:
    explicit VideoEncoder(QObject *parent = nullptr);
    virtual ~VideoEncoder();

    bool init(int w, int h, AVPixelFormat fmt, int fps);

    void on_frame(uint8_t * data, int linesize, int64_t pts);

private:
    int scale_frame(uint8_t * in_data, int in_linesize);

signals:

public slots:

private:
    AVCodec * _codec = nullptr;
    AVCodecContext * _codectx = nullptr;
    AVFrame * _frame = nullptr;
    AVPacket * _pkt = nullptr;
    AVPixelFormat _raw_pix_fmt = AV_PIX_FMT_NONE;
    SwsContext * _sws_context = nullptr;
};

#endif // VIDEOENCODER_H

#ifndef CAMERAHELPER_H
#define CAMERAHELPER_H

#include "dshowcapture.hpp"
#include "videorender.h"
#include "videoencoder.h"

#include <QObject>
#include <QList>
#include <QString>

class CameraHelper : public QObject
{
    Q_OBJECT

public:
    struct CameraInfo
    {
        QString name = "";
        QString path = "";
    };

public:
    explicit CameraHelper(VideoRender *render, QObject *parent = nullptr);
    virtual ~CameraHelper();

public:
    void syncCameraList();

    void on_video_data(const DShow::VideoConfig &config, unsigned char *data,
                             size_t size, long long startTime, long long stopTime);

signals:
    void changeCameraList(QList<QString> camera_list);
    void setStreamUrl(QString url);

public slots:
    void cameraSelected(QString camera);
    void liveBegined();
    void streamUrlGot(QString url);

private:
    QMap<QString, CameraInfo> _all_camera_info_map = {};
    QString _selected_camera = "";

    DShow::Device * _ds_dev = nullptr;

    VideoRender * _render = nullptr;

    bool _encoder_inited = false;
    VideoEncoder * _encoder = nullptr;

    int64_t _frame_seq = 0;
};

#endif // CAMERAHELPER_H

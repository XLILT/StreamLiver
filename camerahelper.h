#ifndef CAMERAHELPER_H
#define CAMERAHELPER_H

#include "dshowcapture.hpp"
#include "videorender.h"

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

public:
    void syncCameraList();

signals:
    void changeCameraList(QList<QString> camera_list);

public slots:
    void cameraSelected(QString camera);
    void liveBegined();

private:
    QMap<QString, CameraInfo> _all_camera_info_map = {};
    QString _selected_camera = "";

    DShow::Device * _ds_dev = nullptr;

    VideoRender * _render = nullptr;
};

#endif // CAMERAHELPER_H

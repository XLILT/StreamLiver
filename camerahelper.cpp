#include "camerahelper.h"

#include <QDebug>
#include <QMessageBox>

using namespace DShow;

CameraHelper::CameraHelper(VideoRender *render, QObject *parent) : QObject(parent),
    _ds_dev(new Device(InitGraph::True)),
    _render(render)
{
}

void CameraHelper::syncCameraList()
{
    using namespace DShow;

    std::vector<VideoDevice> vdevices;

    Device::EnumVideoDevices(vdevices);

    QList<QString> all_cameras;

    for(const auto & vdev : vdevices)
    {
        auto display_name = QString::fromStdWString(vdev.name + L":" + vdev.path);
        all_cameras.append(display_name);

        _all_camera_info_map[display_name] = {QString::fromStdWString(vdev.name), QString::fromStdWString(vdev.path)};
    }

    emit changeCameraList(all_cameras);
}

void CameraHelper::cameraSelected(QString camera)
{
    _selected_camera = camera;
}

void CameraHelper::liveBegined()
{
    using namespace std::placeholders;

    const auto it = _all_camera_info_map.find(_selected_camera);

    if(it == _all_camera_info_map.cend())
    {
        QMessageBox::warning(nullptr, QStringLiteral("提示"), QStringLiteral("未选择摄像头"));

        return;
    }

    const auto & caminfo = it.value();

    VideoConfig vconf;

    vconf.name = caminfo.name.toStdWString();
    vconf.path = caminfo.path.toStdWString();
    vconf.callback = std::bind(&VideoRender::on_pkt, _render, _1, _2, _3, _4, _5);

    _ds_dev->SetVideoConfig(&vconf);
    _ds_dev->ConnectFilters();
    _ds_dev->Start();
}

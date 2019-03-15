#pragma warning (disable:4819)

#include "camerahelper.h"

#include <QDebug>
#include <QMessageBox>

extern "C" {
#include "libavutil/pixfmt.h"
}

using namespace DShow;

namespace {

AVPixelFormat toAVPixelFormat(VideoFormat vid_fmt)
{
    switch(vid_fmt)
    {
    case VideoFormat::YUY2:
        return AV_PIX_FMT_YUYV422;
    default:
        break;
    }

    return AV_PIX_FMT_NONE;
}

}

CameraHelper::CameraHelper(VideoRender *render, QObject *parent) : QObject(parent),
    _ds_dev(new Device(InitGraph::True)),
    _render(render),
    _encoder(new ::VideoEncoder)
{
    connect(this, SIGNAL(setStreamUrl(QString)), _encoder, SLOT(streamUrlGot(QString)));
}

CameraHelper::~CameraHelper()
{
    delete _encoder;
    delete _ds_dev;
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
    vconf.callback = std::bind(&CameraHelper::on_video_data, this, _1, _2, _3, _4, _5);

    _ds_dev->SetVideoConfig(&vconf);
    _ds_dev->ConnectFilters();
    _ds_dev->Start();
}

void CameraHelper::on_video_data(const DShow::VideoConfig &config, unsigned char *data,
                         size_t size, long long startTime, long long stopTime)
{
    // static int i = 0;
    // qDebug() << i++ << " -> " << startTime << " " << stopTime;

    _render->on_pkt(config, data, size, startTime, stopTime);

    if(!_encoder_inited)
    {
        qDebug() << "frameInterval: " << config.frameInterval << " " << 1e7/config.frameInterval;
        _encoder_inited = _encoder->init(config.cx, config.cy, toAVPixelFormat(config.format), 1e7/config.frameInterval);
    }

    // _encoder->on_frame(data, (int)size, _frame_seq++ * config.frameInterval / 1e4);
    _encoder->on_frame(data, (int)size, _frame_seq++);

    // ++_frame_seq;
}

void CameraHelper::streamUrlGot(QString url)
{
    emit setStreamUrl(url);
}

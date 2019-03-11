#include "mainwindow.h"

#include <QQmlContext>
#include <thread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      _central_wid(new CentralWidget(this)),
      _camera_helper(new CameraHelper(_central_wid->video_render(), this))
{
    // winId();
    // _central_wid->bindVideoWindow(windowHandle());

    setCentralWidget(_central_wid);

    // resize(1024, 768);
    // resize(1820, 980);

    connect(_camera_helper, SIGNAL(changeCameraList(QList<QString>)),
            _central_wid, SLOT(cameraListChanged(QList<QString>)));

    connect(_central_wid, SIGNAL(selectCamera(QString)), _camera_helper, SLOT(cameraSelected(QString)));
    connect(_central_wid, SIGNAL(beginLive()), _camera_helper, SLOT(liveBegined()));

    _camera_helper->syncCameraList();

    connect(_central_wid, SIGNAL(changeWindowSize(int,int)), this, SLOT(windowSizeChanged(int,int)));
}

MainWindow::~MainWindow()
{

}

void MainWindow::windowSizeChanged(int w, int h)
{
    // std::this_thread::sleep_for(std::chrono::seconds(5));

    // qDebug() << "windowSizeChanged" << w << " " << h;

    resize(w, h);
}

CentralWidget::CentralWidget(QWidget *parent)
    : QWidget(parent),
      _menu_region(new QQuickWidget(this)),
      _video_render(new VideoRender(this)),
      _layout(new QVBoxLayout),
      _camera_list(new QList<QString>)
{
    _menu_region->setResizeMode(QQuickWidget::SizeRootObjectToView);
    _menu_region->setSource(QUrl("qrc:/main.qml"));

    _layout->addWidget(_menu_region);
    _layout->addWidget(_video_render);
    // _layout->setSpacing(10);
    // _layout->setContentsMargins(10, 10, 10, 10);
    // _layout->setMargin(0);

    setLayout(_layout);

    // QML 与 Qt Widgets 通信
    QObject * obj_root = (QObject*)_menu_region->rootObject();

    if (obj_root != NULL)
    {
        connect(this, SIGNAL(changeCameraList()), obj_root, SIGNAL(cameraListChanged()));

        connect(_video_render, SIGNAL(getVideoSize(int,int)), this, SLOT(videoSizeGot(int,int)));

        connect(obj_root, SIGNAL(selectCamera(QString)), this, SLOT(cameraSelected(QString)));
        connect(obj_root, SIGNAL(beginLive()), this, SLOT(liveBegined()));
    }

    _menu_region->rootContext()->setContextProperty("central_widget", this);
}

CentralWidget::~CentralWidget()
{}

QList<QString> CentralWidget::cameraList()
{
    return *_camera_list;
}

void CentralWidget::cameraListChanged(QList<QString> cam_list)
{
    *_camera_list = cam_list;

    emit changeCameraList();
}

void CentralWidget::cameraSelected(QString camera)
{
    emit selectCamera(camera);
}

void CentralWidget::liveBegined()
{
    emit beginLive();
}

void CentralWidget::videoSizeGot(int w, int h)
{
    // std::this_thread::sleep_for(std::chrono::seconds(5));

    // qDebug() << "videoSizeGot " << w << " " << h;

    _video_render->setFixedSize(w, h);

    emit changeWindowSize(std::max(_menu_region->width(), _video_render->width()) + 50,
                          _menu_region->height() + _video_render->height() + 50);
}

//void CentralWidget::bindVideoWindow(QWindow * win)
//{
//    qDebug() << win << " " << win->isTopLevel();

//    _video_render->winId();

//    qDebug() <<_video_render->windowHandle()->isTopLevel();

//    _video_render->windowHandle()->setParent(win);

//    qDebug() <<_video_render->windowHandle()->isTopLevel();

//    video_render()->bindWindow();
//}

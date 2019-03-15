#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QQuickWidget>
#include <QWidget>
#include <QVBoxLayout>
#include "videorender.h"
#include "camerahelper.h"

#include <QList>
#include <QString>

class CentralWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QList<QString> cameraList READ cameraList)

public:
    CentralWidget(QWidget *parent = 0);
    virtual ~CentralWidget();

    VideoRender * video_render()
    {
        return _video_render;
    }

//    void bindVideoWindow(QWindow * win);

public slots:
    void cameraSelected(QString camera);
    void liveBegined();
    void streamUrlGot(QString url);

    void cameraListChanged(QList<QString> cam_list);

    void videoSizeGot(int w, int h);

signals:
    void selectCamera(QString camera);
    void beginLive();
    void setStreamUrl(QString url);

    void changeCameraList();

    void changeWindowSize(int w, int h);

public:
    QList<QString> cameraList();

private:
    QQuickWidget * _menu_region;    
    VideoRender * _video_render;

    QVBoxLayout * _layout;

    QList<QString> * _camera_list;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void windowSizeChanged(int w, int h);

private:
    CentralWidget * _central_wid;    
    CameraHelper * _camera_helper;
};

#endif // MAINWINDOW_H

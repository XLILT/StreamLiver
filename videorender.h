#ifndef VIDEORENDER_H
#define VIDEORENDER_H

#include "dshowcapture.hpp"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

#include <QWidget>

class VideoRender : public QWidget
{
    Q_OBJECT
public:
    explicit VideoRender(QWidget *parent = nullptr);

public:
//    void bindWindow();

public:
    void on_pkt(const DShow::VideoConfig &config, unsigned char * data, size_t size,
                long long startTime, long long stopTime);

signals:
    void getVideoSize(int w, int h);

public slots:

private:
    SDL_Window * _sdl_screen = nullptr;
    SDL_Renderer * _sdl_renderer = nullptr;
    SDL_Texture * _sdl_texture = nullptr;

    int _width = 0;
    int _height = 0;
};

#endif // VIDEORENDER_H

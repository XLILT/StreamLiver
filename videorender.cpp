#include "videorender.h"

#include "SDL.h"
#include <QDebug>
#include <thread>

using namespace DShow;

namespace {

uint32_t dshow_fmt_to_sdl(VideoFormat fmt)
{
    switch (fmt) {
    case VideoFormat::YUY2:
        return SDL_PIXELFORMAT_YUY2;
    default:
        return SDL_PIXELFORMAT_UNKNOWN;
    }
}

}

VideoRender::VideoRender(QWidget *parent) : QWidget(parent)
{
    SDL_Init(SDL_INIT_EVERYTHING);

    _sdl_screen = SDL_CreateWindowFrom((const void *)(winId()));
}

VideoRender::~VideoRender()
{
    SDL_DestroyTexture(_sdl_texture);
    SDL_DestroyRenderer(_sdl_renderer);
    SDL_DestroyWindow(_sdl_screen);
}

void VideoRender::on_pkt(const DShow::VideoConfig &config, unsigned char *data,
                         size_t size, long long startTime, long long stopTime)
{
    if(!_sdl_texture)
    {
        _width = config.cx;
        _height = config.cy;

        emit getVideoSize(_width, _height);

        uint32_t pixformat = dshow_fmt_to_sdl(config.internalFormat);

        _sdl_renderer = SDL_CreateRenderer(_sdl_screen, -1, SDL_RENDERER_SOFTWARE);
        _sdl_texture = SDL_CreateTexture(_sdl_renderer, pixformat, SDL_TEXTUREACCESS_STREAMING, _width, _height);

        (void)size;
        (void)startTime;
        (void)stopTime;
    }

    SDL_Rect sdl_rect;

    sdl_rect.x = 0;
    sdl_rect.y = 0;
    sdl_rect.w = _width;
    sdl_rect.h = _height;

    SDL_UpdateTexture(_sdl_texture, &sdl_rect, data, _width * 2);
    SDL_RenderClear(_sdl_renderer);
    SDL_RenderCopy(_sdl_renderer, _sdl_texture, NULL, &sdl_rect);
    SDL_RenderPresent(_sdl_renderer);
}

//void VideoRender::bindWindow()
//{
//    _sdl_screen = SDL_CreateWindowFrom((const void *)(winId()));
//}

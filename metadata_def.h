#ifndef METADATA_DEF_H
#define METADATA_DEF_H

#include <string>

struct MetaData
{
    int width = 0;
    int height = 0;
    std::string video_codec_id = "";
    int video_bitrate = 0;
    double fps = 0.0;

    std::string audio_codec_id = "";
    int audio_bitrate = 0;
    int samplerate = 0;
    int audio_channels = 0;
};

#endif // METADATA_DEF_H

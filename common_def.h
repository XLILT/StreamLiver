#ifndef COMMON_DEF_H
#define COMMON_DEF_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    OBS_NAL_UNKNOWN   = 0,
    OBS_NAL_SLICE     = 1,
    OBS_NAL_SLICE_DPA = 2,
    OBS_NAL_SLICE_DPB = 3,
    OBS_NAL_SLICE_DPC = 4,
    OBS_NAL_SLICE_IDR = 5,
    OBS_NAL_SEI       = 6,
    OBS_NAL_SPS       = 7,
    OBS_NAL_PPS       = 8,
    OBS_NAL_AUD       = 9,
    OBS_NAL_FILLER    = 12,
};

/** Specifies the encoder type */
enum obs_encoder_type {
    OBS_ENCODER_AUDIO, /**< The encoder provides an audio codec */
    OBS_ENCODER_VIDEO  /**< The encoder provides a video codec */
};

struct encoder_packet
{
    uint8_t               *data;        /**< Packet data */
    size_t                size;         /**< Packet size */

    int64_t               pts;          /**< Presentation timestamp */
    int64_t               dts;          /**< Decode timestamp */

    int32_t               timebase_num; /**< Timebase numerator */
    int32_t               timebase_den; /**< Timebase denominator */

    enum obs_encoder_type type;         /**< Encoder type */

    bool                  keyframe;     /**< Is a keyframe */

    /* ---------------------------------------------------------------- */
    /* Internal video variables (will be parsed automatically) */

    /* DTS in microseconds */
    int64_t               dts_usec;
    /* System DTS in microseconds */
    int64_t               sys_dts_usec;

    /** Audio track index (used with outputs) */
    // size_t                track_idx;

    /** Encoder from which the track originated from */
    //obs_encoder_t         *encoder;
};

inline uint8_t * write_16b(uint8_t *buf, uint16_t val)
{
    *buf++ = val >> 8;
    *buf++ = (uint8_t)val;
    return buf;
}

inline uint8_t * write_24b(uint8_t *buf, uint32_t val)
{
    buf = write_16b(buf, val >> 8);
    *buf++ = val;
    return buf;
}

inline uint8_t * write_32b(uint8_t *buf, uint32_t val)
{
    return write_16b(write_16b(buf, val >> 16), val);
}

#ifdef __cplusplus
}
#endif

#endif // COMMON_DEF_H

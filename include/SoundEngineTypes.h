/*
*   SoundEngine Types Header For SoundEngine Library by BYuM
*   Author : BYuM
*   Project Started : May 26, 2026
*   Last Updated : May 28, 2026
*   Repository : https://github.com/BYuM-051/SoundEngine.git
*   This header defines the types used in the SoundEngine library.
*/

#ifndef _SOUND_ENGINE_TYPES_INCLUDED_

#define _SOUND_ENGINE_TYPES_INCLUDED_

#include <stdlib.h>
#include <stdint.h>

#include "SoundEngine.h"

/*
*   SoundEngine Raw Sound Structure
*   TODO : add descriptions
*/
typedef struct
{
    const int16_t* soundArray;
    const size_t soundLength;
} RawSoundStruct;

/*
*   SoundEngine Buffer Structure
*   TODO : add descriptions
*/
struct SoundEngine_Buffer_t
{
    const int16_t* soundArray;
    const size_t soundLength;
    size_t currentPlay = 0;
    
    SoundEngine_Buffer_t(const int16_t* array, size_t length)
    : soundArray(array), soundLength(length), currentPlay(0) {}
};

/*
*   SoundEngine Pin Configuration Structure
*   TODO : add descriptions
*/
typedef struct
{
    int lrcPin;
    int bclkPin;
    int dOutPin;
} SoundEngine_PinConfig_t;

#if defined(ESP32)
/*
*   SoundEngine I2S Configuration Structure For ESP32
*   TODO : add descriptions
*/
typedef struct
{
    uint32_t sample_rate = 44100; // default sample rate as 44.1kHz
    i2s_bits_per_sample_t bps = I2S_BITS_PER_SAMPLE_16BIT;
    i2s_channel_fmt_t channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
    i2s_comm_format_t communication_format = I2S_COMM_FORMAT_I2S_MSB;
    int intr_alloc_flags = (ESP_INTR_FLAG_LEVEL1 | ESP_INTR_FLAG_IRAM);
    int dma_buf_count = 4;
    int dma_buf_len = 256;
    bool use_apll = false;
    bool tx_desc_auto_clear = true;
    int fixed_mclk = 0;
    i2s_port_t i2s_port = I2S_NUM_0;
    int queue_size = 0;
    void *i2s_queue = NULL;
} SoundEngine_I2SConfig_t;
#elif defined(STM32F4xx)
/*
*   SoundEngine I2S Configuration Structure For STM32F4xx
*   TODO : add descriptions
*/
typedef struct
{
    //TODO : add I2S configuration fields for STM32F4xx platform

} SoundEngine_I2SConfig_t;
#endif

#endif

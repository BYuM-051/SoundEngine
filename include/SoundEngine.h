/*
*   SoundEngine - A lightweight i2s audio engine for microcontrollers
*   Author : BYuM
*   Project Started : May 26, 2026
*   Last Updated : May 28, 2026
*   Repository : https://github.com/BYuM-051/SoundEngine.git
*   License : MIT
*   This library provides a simple way to mix and play multiple audio samples simultaneously on microcontrollers with I2S support.
*   
*   Supported Platforms:
*   - ESP32 (tested with WaveShare ESP32-S3-Touch_LCD-3.49)
*   - STM32F4 (tested with Weact stdio STM32F412RET6 black pill)
*
*/

#ifndef _SOUND_ENGINE_INCLUDED_
#define _SOUND_ENGINE_INCLUDED_

#ifndef SOUNDENGINE_MIX_LITE
#define SOUNDENGINE_MIX_LITE 0
#endif
#ifndef SOUND_ENGINE_TICK_TO_MS
#define SOUND_ENGINE_TICK_TO_MS 10U
#endif
#ifndef SAMPLE_RATE
#define SAMPLE_RATE 44100.0
#endif
#ifndef TICK_MS
#define TICK_MS ((double)(SOUND_ENGINE_TICK_TO_MS))
#endif
#ifndef SAMPLES_PER_TICK
#define SAMPLES_PER_TICK ((size_t)((SAMPLE_RATE) * (TICK_MS) / 1000.0))
#endif

#include <cstring>
#include <list>
#include <cstdlib>

#if !SOUNDENGINE_MIX_LITE
#include <cmath>
#endif

#if defined(ESP32)
#include "driver/i2s.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#elif defined(STM32F4xx)
extern "C" 
{
#include "FreeRTOS.h"
#include "task.h"
}

#else
#error "Unsupported platform. read the README.md for supported platforms."
#endif

#include "SoundEngineTypes.h"

/*
*   SoundEngine Main Class
*   TODO : add descriptions
*/
class SoundEngine
{
public :
    static SoundEngine* SoundEngineFactory(SoundEngine_PinConfig_t pinConfig, SoundEngine_I2SConfig_t i2sConfig);
    bool enqueSound(const RawSoundStruct* sound);

private :
    std::list<SoundEngine_Buffer_t*> soundQueue;

    SoundEngine_I2SConfig_t* i2sConfig;

    SoundEngine(SoundEngine_PinConfig_t pinConfig, SoundEngine_I2SConfig_t i2sConfig);
    void setupI2S(SoundEngine_PinConfig_t pinConfig, SoundEngine_I2SConfig_t i2sConfig);

    static void soundEngineThreadWrapper(void* param);
    void soundEngineThread();

    #if SOUNDENGINE_MIX_LITE
    inline static int16_t mixSamples(int16_t a, int16_t b)
    {
        constexpr int32_t SAFE_INT16_MIN = -32768;
        constexpr int32_t SAFE_INT16_MAX = 32767;
        return  a < 0 && b < 0 ?
                a + b + ((int32_t) a * b ) / SAFE_INT16_MIN :
                a > 0 && b > 0 ?
                a + b - ((int32_t) a * b ) / SAFE_INT16_MAX :
                a + b;
    }
    #else
    inline static int16_t mixSamples(int16_t a, int16_t b)
    {
        float fa = static_cast<float>(a) / 32768.0f;
        float fb = static_cast<float>(b) / 32768.0f;
        
        float mixed = (fa + fb) / 2.0f;  // or just (fa + fb) * 0.5f
        
        // optional soft saturation (gain = 1.5 정도까지 실험 가능)
        float gain = 1.0f;
        mixed = tanhf(mixed * gain);

        return static_cast<int16_t>(mixed * 32767.0f);
    }
    #endif
};

#endif

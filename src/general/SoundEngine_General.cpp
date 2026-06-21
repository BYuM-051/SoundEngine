/*
*   Created May28, 2026 BYuM
*   Code Implementation For functionality common to all platforms supported by SoundEngine library.
*
*   Last Updated : May 28, 2026
*/


#include "SoundEngine.h"

#if defined(SOUNDENGINE_DEBUG)
    #include <Arduino.h>
    #define soundEngineLog(msg) Serial.println(msg)
#else
    #define soundEngineLog(msg)
#endif

namespace
{
    static const char* TAG = "SoundEngine_General";
}

SoundEngine* SoundEngine::SoundEngineFactory(SoundEngine_PinConfig_t pinConfig, SoundEngine_I2SConfig_t i2sConfig)
{
    soundEngineLog("Creating SoundEngine instance...");
    SoundEngine* engine = new SoundEngine(pinConfig, i2sConfig);
    if(engine == nullptr)
    {
        soundEngineLog("Failed to create SoundEngine instance");
        configASSERT(0); // Failed to create SoundEngine instance
    }
    soundEngineLog("SoundEngine instance created successfully");
    return engine;
}

SoundEngine::SoundEngine(SoundEngine_PinConfig_t pinConfig, SoundEngine_I2SConfig_t i2sConfig)
{
    this->setupI2S(pinConfig, i2sConfig);

    BaseType_t result = xTaskCreate
    (
        soundEngineThreadWrapper,
        "SoundEngine",
        8196, //TODO : optimize stack size
        this,
        3,
        NULL
    );

    if(result != pdPASS)
    {
        soundEngineLog("Failed to create SoundEngine task");
        configASSERT(0); // Failed to create task
    }
    soundEngineLog("SoundEngine task created successfully");
}

/*
*   SoundEngine Thread Wrapper Function
*   The function for wrapping sound engine thread to maximize compatibility with different platforms and RTOSes.
*/
void SoundEngine::soundEngineThreadWrapper(void* param)
{
    SoundEngine* self = static_cast<SoundEngine*>(param);
    self->soundEngineThread();
}

/*
*   SoundEngine Enqueue Sound Function
*   The function for enqueuing sound to the sound engine. It takes a RawSoundStruct pointer as input and returns a boolean indicating success or failure.
*/
bool SoundEngine::enqueSound(const RawSoundStruct* sound)
{
    SoundEngine_Buffer_t* buffer = new SoundEngine_Buffer_t(sound->soundArray, sound->soundLength);
    if(buffer == nullptr)
    {
        soundEngineLog("Failed to allocate memory for sound buffer");
        return false;
    }
    this->soundQueue.push_back(buffer);
    soundEngineLog("Sound enqueued successfully");
    return true;
}

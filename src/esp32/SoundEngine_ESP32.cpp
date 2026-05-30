/*
*   Created May28, 2026 BYuM
*   Code Implementation For ESP32 
*
*   --Testd boards--
*   - WaveShare ESP32-S3-Touch_LCD-3.49 (ESP32-S3R8 / ES8311)
*   - Arduino nano ESP32 (ESP32-S3 / max98357a)
*   
*   Last Updated : May 28, 2026
*/

#if defined(ESP32)
#include "SoundEngine.h"

namespace
{
}

void SoundEngine::setupI2S(SoundEngine_PinConfig_t pinConfig, SoundEngine_I2SConfig_t i2sConfig)
{
    esp_err_t returnValue;

    //making configuration struct for I2S driver
    this->i2sConfig = new SoundEngine_I2SConfig_t(i2sConfig); // store i2sConfig for later use in soundEngineThread
    const i2s_config_t config =
    {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = i2sConfig.sample_rate,
        .bits_per_sample = i2sConfig.bps,
        .channel_format = i2sConfig.channel_format,
        .communication_format = i2sConfig.communication_format,
        .intr_alloc_flags = i2sConfig.intr_alloc_flags,
        .dma_buf_count = i2sConfig.dma_buf_count,
        .dma_buf_len = i2sConfig.dma_buf_len,
        .use_apll = i2sConfig.use_apll,
        .tx_desc_auto_clear = i2sConfig.tx_desc_auto_clear,
        .fixed_mclk = i2sConfig.fixed_mclk
    };

    //trying to install I2S driver
    returnValue = i2s_driver_install
    (
        i2sConfig.i2s_port,
        &config,
        i2sConfig.queue_size,
        i2sConfig.i2s_queue
    );
    if(returnValue != ESP_OK)
    {
        #ifdef SOUNDENGINE_DEBUG
            Serial.println("Failed to install I2S driver");
        #endif
        configASSERT(0); // Failed to install I2S driver
    }

    //mapping I2S signals to GPIOs
    i2s_pin_config_t pin_config = 
    {
        .bck_io_num = pinConfig.bclkPin,
        .ws_io_num = pinConfig.lrcPin,
        .data_out_num = pinConfig.dOutPin,
        .data_in_num = I2S_PIN_NO_CHANGE
    };
    returnValue = i2s_set_pin(i2sConfig.i2s_port, &pin_config);
    if(returnValue != ESP_OK)
    {
        #ifdef SOUNDENGINE_DEBUG
            Serial.println("Failed to set I2S pins");
        #endif
        configASSERT(0); // Failed to set I2S pins
    }

    //setting i2s clock
    returnValue = i2s_set_clk
    (
        i2sConfig.i2s_port,
        i2sConfig.sample_rate,
        i2sConfig.bps,
        i2sConfig.channel_format == I2S_CHANNEL_FMT_ONLY_LEFT || i2sConfig.channel_format == I2S_CHANNEL_FMT_ONLY_RIGHT ? I2S_CHANNEL_MONO : I2S_CHANNEL_STEREO
    );
    if(returnValue != ESP_OK)
    {
        #ifdef SOUNDENGINE_DEBUG
            Serial.println("Failed to set I2S clock");
        #endif
        configASSERT(0); // Failed to set I2S clock
    }

    return;
}

//TODO : optimize this function refer to USSEO Project
void SoundEngine::soundEngineThread()
{
    TickType_t lastWakeTime = xTaskGetTickCount();
    int16_t mixBuffer[SAMPLES_PER_TICK];

    while(true)
    {
        if(!soundQueue.empty())
        {
            size_t samplesToMix = 0;
            for(auto i = soundQueue.begin() ; i != soundQueue.end() ; )
            {
                SoundEngine_Buffer_t* buffer = *i;

                size_t remaining = buffer->soundLength - buffer->currentPlay;
                samplesToMix = std::min(SAMPLES_PER_TICK, remaining);

                for(size_t j = 0 ; j < samplesToMix ; ++j)
                {
                    mixBuffer[j] = mixSamples(mixBuffer[j], buffer->soundArray[buffer->currentPlay + j]);
                }
                buffer->currentPlay += samplesToMix;

                if(buffer->currentPlay >= buffer->soundLength)
                {
                    i = soundQueue.erase(i);
                    delete buffer;
                }
                else
                {
                    ++i;
                }
            }

            size_t bytesWritten;
            
            i2s_write
            (
                this->i2sConfig->i2s_port,
                mixBuffer,
                samplesToMix * sizeof(int16_t),
                &bytesWritten,
                portMAX_DELAY
            );
            memset(mixBuffer, 0, sizeof(mixBuffer));
            
            xTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(SOUND_ENGINE_TICK_TO_MS));
        }
    }
}



#endif
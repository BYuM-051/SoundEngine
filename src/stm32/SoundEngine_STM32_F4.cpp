/*
*   Not Ported Yet.
*   Code Implementation For STM32F4 
*   
*   --Testd boards--
*   - Weact stdio STM32F412RET6 black pill
*
*   Last Updated : Not Ported Yet.
*/

#if defined (STM32F4xx)

#include "SoundEngine.h"

#include "stm32f4xx_hal.h"

extern "C" {
#include "semphr.h"
}

extern "C" __attribute__((weak)) GPIO_TypeDef* SoundEngine_STM32F4_GPIOPort(int pin)
{
    (void)pin;
    return GPIOB;
}

namespace
{
constexpr size_t OUTPUT_SAMPLES = 256;
constexpr uint32_t I2S_TIMEOUT_MS = 1000;

I2S_HandleTypeDef i2sHandle;
SemaphoreHandle_t queueMutex = nullptr;

uint16_t gpioPinFromArgument(int pin)
{
    return pin >= 0 && pin <= 15 ? static_cast<uint16_t>(1u << pin) : static_cast<uint16_t>(pin);
}

void enableGpioClock(GPIO_TypeDef* port)
{
    if (port == GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE();
    else if (port == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();
    else if (port == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
    else if (port == GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE();
    else if (port == GPIOE) __HAL_RCC_GPIOE_CLK_ENABLE();
#if defined(GPIOF)
    else if (port == GPIOF) __HAL_RCC_GPIOF_CLK_ENABLE();
#endif
#if defined(GPIOG)
    else if (port == GPIOG) __HAL_RCC_GPIOG_CLK_ENABLE();
#endif
#if defined(GPIOH)
    else if (port == GPIOH) __HAL_RCC_GPIOH_CLK_ENABLE();
#endif
#if defined(GPIOI)
    else if (port == GPIOI) __HAL_RCC_GPIOI_CLK_ENABLE();
#endif
}

void configureI2SPin(int pin)
{
    GPIO_TypeDef* port = SoundEngine_STM32F4_GPIOPort(pin);
    enableGpioClock(port);

    GPIO_InitTypeDef gpio = {};
    gpio.Pin = gpioPinFromArgument(pin);
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = GPIO_AF6_SPI3;
    HAL_GPIO_Init(port, &gpio);
}
}

SoundEngine::SoundEngine(int lrcPin, int bclkPin, int dOutPin)
{
    setupI2S(lrcPin, bclkPin, dOutPin);

    if (queueMutex == nullptr)
    {
        queueMutex = xSemaphoreCreateMutex();
    }

    if (queueMutex != nullptr)
    {
        xTaskCreate(
            soundEngineThreadWrapper,
            "SoundEngine",
            512,
            this,
            tskIDLE_PRIORITY + 2,
            nullptr);
    }
}

bool SoundEngine::enqueSound(const RawSoundStruct* sound)
{
    if (sound == nullptr || sound->soundArray == nullptr || sound->soundLength == 0 || queueMutex == nullptr)
    {
        return false;
    }

    SoundEngine_Buffer_t* buffer = new SoundEngine_Buffer_t(sound->soundArray, sound->soundLength);
    if (buffer == nullptr)
    {
        return false;
    }

    xSemaphoreTake(queueMutex, portMAX_DELAY);
    soundQueue.push_back(buffer);
    xSemaphoreGive(queueMutex);
    return true;
}

void SoundEngine::setupI2S(int lrcPin, int bclkPin, int dOutPin)
{
    __HAL_RCC_SPI3_CLK_ENABLE();

    configureI2SPin(lrcPin);
    configureI2SPin(bclkPin);
    configureI2SPin(dOutPin);

    i2sHandle.Instance = SPI3;
    i2sHandle.Init.Mode = I2S_MODE_MASTER_TX;
    i2sHandle.Init.Standard = I2S_STANDARD_PHILIPS;
    i2sHandle.Init.DataFormat = I2S_DATAFORMAT_16B;
    i2sHandle.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
    i2sHandle.Init.AudioFreq = I2S_AUDIOFREQ_44K;
    i2sHandle.Init.CPOL = I2S_CPOL_LOW;
    i2sHandle.Init.ClockSource = I2S_CLOCK_PLL;
    i2sHandle.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;

    HAL_I2S_DeInit(&i2sHandle);
    HAL_I2S_Init(&i2sHandle);
}

void SoundEngine::soundEngineThreadWrapper(void* param)
{
    static_cast<SoundEngine*>(param)->soundEngineTask();
}

void SoundEngine::soundEngineTask()
{
    if (queueMutex == nullptr)
    {
        vTaskDelete(nullptr);
    }

    int16_t mixed[OUTPUT_SAMPLES];

    for (;;)
    {
        bool hasAudio = false;

        memset(mixed, 0, sizeof(mixed));

        xSemaphoreTake(queueMutex, portMAX_DELAY);
        for (auto it = soundQueue.begin(); it != soundQueue.end();)
        {
            SoundEngine_Buffer_t* buffer = *it;
            hasAudio = true;

            for (size_t sample = 0; sample < OUTPUT_SAMPLES && buffer->currentPlay < buffer->soundLength; ++sample)
            {
                mixed[sample] = mixSamples(mixed[sample], buffer->soundArray[buffer->currentPlay++]);
            }

            if (buffer->currentPlay >= buffer->soundLength)
            {
                it = soundQueue.erase(it);
                delete buffer;
            }
            else
            {
                ++it;
            }
        }
        xSemaphoreGive(queueMutex);

        HAL_I2S_Transmit(
            &i2sHandle,
            reinterpret_cast<uint16_t*>(mixed),
            OUTPUT_SAMPLES,
            I2S_TIMEOUT_MS);

        if (!hasAudio)
        {
            taskYIELD();
        }
    }
}

#endif

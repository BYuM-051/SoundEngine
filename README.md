![License](https://img.shields.io/badge/license-MIT-green)

# SoundEngine

SoundEngine is an audio playback and mixing engine designed for MCUs that support I2S. It is intended to simplify audio playback in **FreeRTOS** environments while remaining lightweight and easy to integrate into embedded projects.

Currently supported platforms include:

- ✅ ESP32-S3
- 🚧 STM32 F Series (In Progress)
- 🚧 More platforms coming soon

## Features

- Audio mixing engine
- Queue-based sound playback
- Designed for FreeRTOS
- I2S output
- Lightweight implementation

### Supported Audio Formats

| Source | Status |
| ------- | :----: |
| Raw PCM Data Array | ✅ |
| WAV File | ✅ |
| SD Card WAV Streaming | 🚧 |
| WiFi Audio Streaming | 🚧 |

---

# How to Use

## Data Types

### `RawSoundStruct`

Represents a single PCM audio resource.

| Member | Type | Description |
| ------- | ---- | ----------- |
| `soundArray` | `const int16_t*` | Pointer to 16-bit PCM sample array |
| `soundLength` | `size_t` | Number of PCM samples in `soundArray` |

---

### `SoundEngine_PinConfig_t`

I2S pin configuration.

| Member | Type | Description |
| ------- | ---- | ----------- |
| `lrcPin` | `int` | I2S LRCLK (Word Select) pin |
| `bclkPin` | `int` | I2S Bit Clock pin |
| `dOutPin` | `int` | I2S Data Output pin (Connect to codec DIN) |

---

## Data Types for ESP32

### `SoundEngine_I2SConfig_t`

Advanced I2S configuration for ESP32.

| Member | Default | Description |
| ------- | :-----: | ----------- |
| `sample_rate` | `44100` | Audio sample rate (44.1 kHz) |
| `bps` | `I2S_BITS_PER_SAMPLE_16BIT` | Audio bit depth |
| `channel_format` | `I2S_CHANNEL_FMT_ONLY_LEFT` | Output channel configuration |
| `communication_format` | `I2S_COMM_FORMAT_I2S_MSB` | I2S communication format |
| `intr_alloc_flags` | `ESP_INTR_FLAG_LEVEL1 \| ESP_INTR_FLAG_IRAM` | Interrupt allocation flags |
| `dma_buf_count` | `4` | Number of DMA buffers |
| `dma_buf_len` | `256` | DMA buffer length (samples) |
| `use_apll` | `false` | Enable Audio PLL |
| `tx_desc_auto_clear` | `true` | Automatically clear DMA descriptor on underflow |
| `fixed_mclk` | `0` | Fixed master clock (0 = disabled) |
| `i2s_port` | `I2S_NUM_0` | I2S peripheral to use |
| `queue_size` | `0` | Event queue size (0 disables event queue) |
| `i2s_queue` | `NULL` | Pointer to event queue handle |

---

## Initialize

---

### `SoundEngine::SoundEngineFactory()`

Creates and initializes a `SoundEngine` instance using the specified I2S pin configuration and ESP32 I2S settings.

```cpp
SoundEngine* SoundEngine::SoundEngineFactory(
    SoundEngine_PinConfig_t pinConfig,
    SoundEngine_I2SConfig_t i2sConfig
);
```

#### Parameters

| Parameter | Description |
| --------- | ----------- |
| `pinConfig` | I2S pin configuration (`LRC`, `BCLK`, `DOUT`) |
| `i2sConfig` | Advanced ESP32 I2S configuration. Use the default values unless customization is required. |

#### Returns

| Return | Description |
| ------ | ----------- |
| `SoundEngine*` | Pointer to the initialized `SoundEngine` instance. Returns `nullptr` if initialization fails. |

#### Example

```cpp
SoundEngine_PinConfig_t pinConfig = {
    .lrcPin = 5,
    .bclkPin = 6,
    .dOutPin = 7
};

SoundEngine_I2SConfig_t i2sConfig;

SoundEngine* soundEngine =
    SoundEngine::SoundEngineFactory(pinConfig, i2sConfig);

if (soundEngine == nullptr)
{
    // Initialization failed
}
```

---

## Mix & Play

---

Queue a sound for playback.

```cpp
bool soundEngine.enqueSound(const RawSoundStruct* sound);
```

### Parameters

| Parameter | Description |
| --------- | ----------- |
| `sound` | Pointer to the PCM sound resource |

### Returns

| Return | Description |
| ------- | ----------- |
| `true` | Successfully queued |
| `false` | Queue is full or failed |

---

# Examples

## ESP32-S3

- MAX98357A Example *(Coming Soon)*
- ES8311 Codec Example *(Coming Soon)*

---

## STM32 F4

Coming Soon.

---

# License

This project is currently under development.
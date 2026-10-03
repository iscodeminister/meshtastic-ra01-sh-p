#ifndef _VARIANT_ESP32C3_SUPER_MINI_RA01SH_P_
#define _VARIANT_ESP32C3_SUPER_MINI_RA01SH_P_

// ESP32-C3 Super Mini + Ai-Thinker Ra-01SH-P. AI_THINKER_RA01SH_P (platformio.ini) caps the SX1262 at +3dBm:
// a higher drive destroys the module's FEM. Do not define SX126X_MAX_POWER here.
#ifndef AI_THINKER_RA01SH_P
#error "Ra-01SH-P variant built without AI_THINKER_RA01SH_P: the SX1262 power clamp would be missing."
#endif

// I2C (Wire) & OLED
#define WIRE_INTERFACES_COUNT (1)
#define I2C_SDA (1)
#define I2C_SCL (2)

#define USE_SSD1306

// No GPS: GPIO21 is RF_EN
#define HAS_GPS 0
#undef GPS_RX_PIN
#undef GPS_TX_PIN

// Button
#define BUTTON_PIN (9) // BOOT button

// LoRa
#define USE_SX1262

#define LORA_DIO0 RADIOLIB_NC
#define LORA_RESET (5)
#define LORA_DIO1 (3)
#define LORA_BUSY (4)
#define LORA_SCK (10)
#define LORA_MISO (6)
#define LORA_MOSI (7)
#define LORA_CS (8)

#define SX126X_CS LORA_CS
#define SX126X_DIO1 LORA_DIO1
#define SX126X_BUSY LORA_BUSY
#define SX126X_RESET LORA_RESET
#define SX126X_DIO2_AS_RF_SWITCH

// Module RF_EN (FEM enable, active high, 10k pull-up on the module)
#define SX126X_POWER_EN (21)

// Ra-01SH-P uses a crystal, not a TCXO: leave SX126X_DIO3_TCXO_VOLTAGE undefined

#endif

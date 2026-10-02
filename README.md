# Meshtastic for ESP32-C3 Super Mini + RA-01SH-P

Custom Meshtastic firmware configuration for an **ESP32-C3 Super Mini**, an
**Ai-Thinker RA-01SH-P** LoRa module, and an optional SSD1306 I2C OLED.
The RA-01SH-P combines an SX1262 with an internal RF front-end amplifier (FEM)
that requires a lower chip transmit-power limit than the stock configuration.

Build environment: **`esp32c3_super_mini_ra01sh_p`**.

## Changes from stock firmware

This comparison uses the stock
[`esp32c3_super_mini`](variants/esp32c3/diy/esp32c3_super_mini/variant.h)
variant in this checkout as its baseline.

| Setting | Stock Super Mini | RA-01SH-P build |
| --- | --- | --- |
| Radio selection | LLCC68, SX1262, SX1268 probing | SX1262 only |
| SX1262 maximum drive | Default +22 dBm | **+3 dBm cap** to protect the FEM |
| Amplifier gain compensation | Default 0 dB | **26 dB estimate** via `TX_GAIN_LORA` |
| RF control | MCU RX enable on GPIO2 | DIO2 RF-switch control; FEM enable on GPIO21 |
| GPS | RX GPIO20, TX GPIO21 | Disabled; GPIO21 reserved for RF_EN |
| OLED I2C | SDA GPIO1, SCL GPIO0 | SDA GPIO1, **SCL GPIO2** |

SPI, RESET, DIO1, and BUSY assignments are unchanged. Both variants use the
`esp32-c3-devkitm-1` board definition, native USB CDC, GPIO9 as the BOOT button,
and optional TCXO probing with a crystal fallback.

The hardware-specific additions are:

- [`variant.h`](variants/esp32c3/diy/esp32c3_super_mini_ra01sh_p/variant.h): wiring, radio selection, RF_EN, and GPS disable.
- [`platformio.ini`](variants/esp32c3/diy/esp32c3_super_mini_ra01sh_p/platformio.ini): separate build environment enabling `AI_THINKER_RA01SH_P`.
- [`src/configuration.h`](src/configuration.h): gain compensation and chip-power cap, enabled only for `AI_THINKER_RA01SH_P` builds. The variant rejects compilation if that flag is missing.

## Wiring

These are ESP32-C3 GPIO numbers, not physical header positions.

| RA-01SH-P signal | Module pin | ESP32-C3 connection |
| --- | --- | --- |
| SCK | 12 | GPIO10 |
| MISO | 13 | GPIO6 |
| MOSI | 14 | GPIO7 |
| NSS / CS | 15 | GPIO8 |
| RESET | 4 | GPIO5 |
| DIO1 | 6 | GPIO3 |
| BUSY | 10 | GPIO4 |
| RF_EN | 11 | GPIO21 |
| 3V3 | 3 | Regulated 3.3 V |
| GND | 2, 9, 16, exposed pad | Common ground |
| VCCPA | 5 | Unconnected for the default module BOM |

| Optional OLED signal | ESP32-C3 connection |
| --- | --- |
| SDA | GPIO1 |
| SCL | GPIO2 |
| VCC | 3.3 V |
| GND | Common ground |

DIO2 RF switching is configured on the SX1262 itself; this variant assigns no
ESP32 GPIO to DIO2. GPIO21 enables the FEM for reception and transmission and
is driven low by the radio sleep path.

GPIO2, GPIO8, and GPIO9 are strapping pins. Attached circuits must preserve
valid reset levels; use 3.3 V OLED pull-ups. See
[Espressif's hardware guidance](https://docs.espressif.com/projects/esp-hardware-design-guidelines/en/latest/esp32c3/schematic-checklist.html).

## Radio power and supply

**Use this custom build rather than the stock Super Mini image.** Ai-Thinker
limits SX1262 drive to **+3 dBm** to prevent FEM damage. The module is rated for
up to +29 dBm RF output.

This build subtracts an estimated 26 dB amplifier gain from the requested power
in normal operation, then caps chip drive at +3 dBm. A 29 dBm request therefore
resolves to 3 dBm chip drive, unless a lower regional limit applies. The existing
licensed-mode path skips gain compensation but still applies the chip-power cap.

**26 dB is an estimate, not a measured power curve.** App-requested output and
chip drive shown in logs are different quantities. Actual output needs
measurement, especially at low settings. The driver has a -9 dBm chip-drive
floor, so arbitrarily low requested RF output cannot be assumed achievable.

- Use a **3.3 V supply with peak capacity above 1 A**, as recommended by Ai-Thinker. Verify the Mini's regulator capacity before using its 3.3 V header.
- Leave VCCPA floating on the default module; do not supply it with 5 V.
- Attach a matched antenna before transmitting. The module covers **803–930 MHz**.
- Power the module before, or together with, RF_EN. RF_EN must not exceed the module supply voltage.

Hardware specifications and pin numbers:
[Ai-Thinker RA-01SH-P V1.0.3 datasheet](https://aithinker-static.oss-cn-shenzhen.aliyuncs.com/docs/Public%20Document%20Center/LoRa/lora/Specification/Ra-01SH-P_V1.0.3%20Specification-20260730A.pdf).

## Build and upload

Install PlatformIO using the upstream
[Meshtastic build instructions](https://meshtastic.org/docs/development/firmware/build).
From the repository root:

```sh
git submodule update --init --recursive
pio run -e esp32c3_super_mini_ra01sh_p
```

Always specify the environment. The root `platformio.ini` still defaults to
`heltec-v3`; the generic `esp32c3_super_mini` environment also lacks the
RA-01SH-P protection.

To upload, replace `YOUR_SERIAL_PORT` with your board's port:

```sh
pio device list
pio run -e esp32c3_super_mini_ra01sh_p -t upload --upload-port YOUR_SERIAL_PORT
pio device monitor --port YOUR_SERIAL_PORT --baud 115200
```

In the Meshtastic client, select the region appropriate for your physical
location and match channel and modem settings with your peers. This variant
does not force a region, channel, or modem preset.

## Validation status

The pin configuration matches the wiring above, and PlatformIO recognizes the
custom environment. A successful firmware build and hardware TX/RX operation
have **not yet been verified** for this configuration.

The oscillator setting tries a 1.8 V TCXO with a crystal fallback. Confirm
successful SX1262 initialization in the boot log. RF output calibration,
supply stability during transmission, and OLED operation remain to be checked
on the assembled device.

Based on [Meshtastic firmware](https://github.com/meshtastic/firmware).
See the upstream [user documentation](https://meshtastic.org/docs/) for general operation.

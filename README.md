# Meshtastic for the Ai-Thinker RA-01SH-P

Meshtastic **2.7.26** (stable) firmware builds for the **Ai-Thinker RA-01SH-P** LoRa module, an
SX1262 with an internal RF front-end amplifier (FEM) rated for +29 dBm.

**Stock Meshtastic destroys this module.** Stock builds drive the SX1262 at up
to +22 dBm, but Ai-Thinker limits the FEM input to **+3 dBm**. These builds hold
the SX1262 at exactly +3 dBm.

| Host board | Build environment |
| --- | --- |
| ESP32-C3 Super Mini (optional SSD1306 OLED) | `esp32c3_super_mini_ra01sh_p` |
| nRF52840 ProMicro / Faketec | `nrf52_promicro_diy_ra01sh_p` |

Do not flash the generic `esp32c3_super_mini` or `nrf52_promicro_diy_tcxo`
images onto a RA-01SH-P.

## Radio settings

Both builds define `AI_THINKER_RA01SH_P`, which enables the following in
[`src/configuration.h`](src/configuration.h):

| Setting | Value |
| --- | --- |
| SX1262 drive | **Fixed at +3 dBm** (`SX126X_MAX_POWER 3`, `LORA_FIXED_TX_POWER`) |
| RF output | about +29 dBm, the module's rated maximum |
| Oscillator | Crystal (no TCXO on DIO3) |
| TX/RX switching | SX1262 DIO2 |
| FEM enable (RF_EN) | MCU pin held high; low only in deep sleep |

**The TX power setting in the Meshtastic app is ignored.** The app shows the
region limit (for example 30 dBm for US, 27 dBm for TW), but the chip is always
driven at +3 dBm. The region's power limit is therefore not enforced: in a
region whose limit is below +29 dBm, use a lower-gain antenna to stay legal.

The ESP32-C3 variant fails to compile if `AI_THINKER_RA01SH_P` is missing.

The boot log confirms the settings:

```
Final Tx power: 3 dBm
SX126X_DIO3_TCXO_VOLTAGE not defined, DIO3 not used as TCXO Vref
SX1262 init success, XTAL
```

If `Final Tx power` ever reports more than 3 dBm, disconnect power.

## Wiring: ESP32-C3 Super Mini

Variant: [`variants/esp32c3/diy/esp32c3_super_mini_ra01sh_p`](variants/esp32c3/diy/esp32c3_super_mini_ra01sh_p/variant.h).
Numbers are ESP32-C3 GPIOs, not header positions.

| RA-01SH-P signal | Module pin | ESP32-C3 |
| --- | --- | --- |
| SCK | 12 | GPIO10 |
| MISO | 13 | GPIO6 |
| MOSI | 14 | GPIO7 |
| NSS | 15 | GPIO8 |
| RESET | 4 | GPIO5 |
| DIO1 | 6 | GPIO3 |
| BUSY | 10 | GPIO4 |
| RF_EN | 11 | GPIO21, or unconnected |
| 3V3 | 3 | 3.3 V supply (see below) |
| GND | 2, 9, 16, pad | Common ground |
| VCCPA | 5 | Unconnected |

| OLED | ESP32-C3 |
| --- | --- |
| SDA | GPIO1 |
| SCL | GPIO2 |

- GPS is disabled; GPIO21 is used for RF_EN.
- GPIO21 is also UART0 TX. The ROM bootloader toggles it briefly at reset, before the radio is configured.
- GPIO2, GPIO8 and GPIO9 are strapping pins and must be high at reset. Use 3.3 V OLED pull-ups.

## Wiring: nRF52840 ProMicro / Faketec

Variant: [`variants/nrf52840/diy/nrf52_promicro_diy_tcxo`](variants/nrf52840/diy/nrf52_promicro_diy_tcxo/variant.h)
(standard ProMicro pinout, same as for an E22 module).

| RA-01SH-P signal | ProMicro / Faketec |
| --- | --- |
| NSS | P1.13 |
| SCK | P1.11 |
| MOSI | P1.15 |
| MISO | P0.02 |
| RESET | P0.09 |
| BUSY | P0.29 |
| DIO1 | P0.10 |
| RF_EN | P0.17 (RXEN pad), or unconnected |

In this build P0.17 holds RF_EN high instead of acting as an RX-enable line, and
the E22 TCXO setting is removed.

## RF_EN

RF_EN has a 10 kΩ pull-up inside the module, so the FEM is on when the pin is
unconnected. Connecting it lets the firmware switch the FEM off in deep sleep
(about 360 µA saved, per the datasheet). It must not exceed the module's 3V3
supply voltage, and 3V3 must be powered before or together with RF_EN.

## Power supply and antenna

- Supply the module from a **3.3 V regulator rated above 1 A**. It draws about 690 mA while transmitting and needs 3.0–3.6 V. The onboard regulators of the Super Mini and ProMicro are not intended for this load: use a separate regulator with a shared ground and a capacitor of 100 µF or more at the module's 3V3 pin.
- Leave VCCPA unconnected; do not supply it with 5 V.
- **Connect the antenna before powering on.** Transmitting into an open antenna port can destroy the FEM.
- Frequency range: 803–930 MHz.

Reference: [RA-01SH-P V1.0.3 datasheet](https://aithinker-static.oss-cn-shenzhen.aliyuncs.com/docs/Public%20Document%20Center/LoRa/lora/Specification/Ra-01SH-P_V1.0.3%20Specification-20260730A.pdf).

## Build and flash

Install PlatformIO as in the
[Meshtastic build instructions](https://meshtastic.org/docs/development/firmware/build),
then from the repository root:

```sh
git submodule update --init --recursive
pio run -e esp32c3_super_mini_ra01sh_p
pio run -e nrf52_promicro_diy_ra01sh_p
```

Always name the environment; the default environment is `heltec-v3`.

**ESP32-C3:**

```sh
pio run -e esp32c3_super_mini_ra01sh_p -t upload --upload-port YOUR_SERIAL_PORT
pio device monitor --port YOUR_SERIAL_PORT --baud 115200
```

**Faketec:** double-tap reset to mount the UF2 drive, then copy
`.pio/build/nrf52_promicro_diy_ra01sh_p/firmware-nrf52_promicro_diy_ra01sh_p-*.uf2`
onto it.

### Build fails with `No module named 'SCons.Tool.FortranCommon'`

The ESP32 platform installs PlatformIO into `~/.platformio/penv` and takes the
newest `pioarduino` release. 6.2.0 requires SCons 4.11.1, while the platform
pins SCons 4.8.1. Pin the environment to 6.1.19:

```sh
uv pip install --python ~/.platformio/penv/bin/python "pioarduino==6.1.19"
~/.platformio/penv/bin/pio run -e esp32c3_super_mini_ra01sh_p
```

The first ESP32-C3 build also compiles the ESP-IDF libraries and needs about
8 GB of free disk space.

## Status

| | ESP32-C3 Super Mini | Faketec |
| --- | --- | --- |
| Firmware builds | Yes | Yes |
| Transmit on hardware | Not tested on 2.7.26 | Not tested |
| Receive on hardware | Not tested on 2.7.26 | Not tested |
| RF output measured | No | No |

The same RA-01SH-P changes were verified on hardware (ESP32-C3: transmit, receive,
channel broadcasts relayed by the mesh) on the `develop` branch.

`TX_GAIN_LORA 26` is derived from the datasheet (+29 dBm out at +3 dBm in) and
affects only log output while drive is fixed.

Based on [Meshtastic firmware](https://github.com/meshtastic/firmware). See the
[Meshtastic documentation](https://meshtastic.org/docs/) for general use.

# FPVCineCam32 v0.8 - Betaflight RC control build

Prototype firmware for an **ESP32-C3 SuperMini** linking a Blackmagic Pocket Cinema Camera 4K to Betaflight 2025.12+.

## What is proven already

- BMPCC 4K BLE scan and secure PIN pairing
- Bond survives ESP32 power cycles
- Automatic reconnect
- Encrypted Blackmagic REC / STOP control
- Camera record-state and timecode notifications
- Stable camera control with development Wi-Fi left on

## v0.8 goal

Prove the next part of the flight chain:

`TX16S -> receiver -> Betaflight -> MSP UART -> ESP32 -> BLE -> BMPCC 4K`

v0.8 adds:

- Fixed ESP32-C3 SuperMini UART profile: **GPIO6 = RX, GPIO7 = TX, 115200 baud**
- Live Betaflight MSP status and API version
- Live CH1-CH16 values in the web page
- MSP diagnostics: RC responses, timeouts, invalid frames and last response time
- User-selectable REC/STOP RC channel, threshold and active direction
- Default mapping remains CH11 / 1500 / above threshold = REC
- Mapping is now shown in the Blackmagic camera section rather than as a hardware setting
- More robust REC/STOP application after camera reconnect/authentication
- Development Wi-Fi remains permanently on for bench diagnostics

## Hardware

- ESP32-C3 SuperMini (4 MB)
- BMPCC 4K
- Betaflight 2025.12+ flight controller

## Wiring

For the ESP32-C3 SuperMini profile, UART pins are intentionally fixed:

| Flight controller | ESP32-C3 SuperMini |
|---|---|
| GND | GND |
| FC TX (spare UART) | GPIO6 (ESP RX) |
| FC RX (same UART) | GPIO7 (ESP TX) |

Power the ESP32 from a suitable regulated supply. **Do not feed raw LiPo voltage to the ESP32.**

In Betaflight Ports, enable **MSP at 115200** on that spare UART. Do not assign Serial RX, GPS, or another function to the same UART.

## v0.8 bench sequence

1. Flash v0.8 and reconnect to `FPVCineCam32-XXXX`, password `fpvcinecam32`.
2. Open `http://192.168.4.1`.
3. Confirm the BMPCC reconnects and web REC/STOP still work.
4. Wire FC TX -> GPIO6, FC RX -> GPIO7, and common GND.
5. Enable MSP 115200 on that FC UART.
6. Watch the Betaflight panel. It should show MSP connected, API version, and live CH1-CH16 values.
7. Move the TX16S switch assigned to the chosen channel and verify its live channel value changes.
8. Choose that channel in the Blackmagic REC/STOP mapping, set threshold/direction, and save.
9. Flip the switch. The BMPCC should start and stop recording.

## OSD

The existing `MSP2_SET_TEXT` Custom Message support remains in the firmware and the OSD test button remains available, but **v0.8 is primarily an RC/MSP control test**. Camera-state-to-goggles OSD is the next phase after switch control is proven.

## Development Wi-Fi

Wi-Fi is intentionally kept on continuously in v0.8 so the live MSP channels and diagnostics remain visible during bench testing. This is not the intended final flight behaviour. Automatic Wi-Fi shutdown will be restored after the MSP/OSD path is proven.

## Build locally

Install VS Code + PlatformIO, then:

```bash
pio run
```

Upload by USB:

```bash
pio run -t upload
```

## Browser flasher

`docs/index.html` uses ESP Web Tools. The existing GitHub Actions workflow builds the firmware and publishes the binaries used by `docs/manifest.json`.

Web Serial does not work from iOS Safari, so use a Mac/PC for browser flashing.

## License

MIT. Blackmagic Design and Betaflight are trademarks/projects of their respective owners. This project is independent and uses publicly documented protocols.

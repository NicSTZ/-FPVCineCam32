# FPVCineCam32 v0.1 — BMPCC 4K + Betaflight

Prototype firmware for an **ESP32-C3 SuperMini** that links a Blackmagic Pocket Cinema Camera 4K to Betaflight 2025.12+.

## v0.1 goals

- Blackmagic BLE scan, secure pairing and remembered bond
- BMPCC 4K REC / STOP control
- Read camera record state + timecode notifications
- Read Betaflight RC channels over MSP on a spare FC UART
- Map one AUX channel to camera REC/STOP
- Write camera status into Betaflight Custom Message 0–3 with `MSP2_SET_TEXT`
- Self-hosted configuration page over ESP32 Wi-Fi
- Browser installer scaffold using ESP Web Tools
- Camera backend abstraction so RED / GoPro / DJI / Insta360 can be added later without rewriting MSP/OSD/UI

## Hardware

- ESP32-C3 SuperMini (4 MB)
- BMPCC 4K
- Betaflight 2025.12+ flight controller

### Wiring

Only three wires are needed:

| Flight controller | ESP32-C3 |
|---|---|
| GND | GND |
| FC TX (spare UART) | configured ESP RX GPIO |
| FC RX (same UART) | configured ESP TX GPIO |

Power the ESP32 from a suitable regulated supply for your specific SuperMini board. **Do not feed raw LiPo voltage to the ESP32.**

In Betaflight Ports, enable **MSP** on that spare UART at 115200. Do not assign Serial RX/GPS/etc. to the same UART.

## First boot

1. Flash firmware.
2. Join Wi-Fi `FPVCineCam32-XXXX`, password `fpvcinecam32`.
3. Browse to `http://192.168.4.1`.
4. Set the ESP RX/TX GPIOs used for the FC UART and save.
5. In Betaflight OSD, place **Custom Message 0** (or the slot chosen in setup).
6. Scan for the BMPCC 4K and select it.
7. The camera should show a six-digit Bluetooth PIN. Enter it on the FPVCineCam32 page.
8. Test REC and STOP on the web page.
9. Move the assigned radio AUX channel through its threshold. The camera should follow it.

The setup AP automatically switches off 90 seconds after boot once a paired camera is connected. This reduces Wi-Fi/BLE coexistence traffic in flight. Hold the **BOOT** button while powering the ESP32 to keep setup Wi-Fi enabled.

## Important v0.1 assumptions / diagnostics

- Default UART pins are GPIO4 RX and GPIO3 TX, but **they are configurable** because C3 SuperMini clones vary.
- Default REC channel is **CH11**, threshold 1500, high=REC.
- The BMPCC secure pairing flow is implemented using NimBLE passkey injection from the web UI.
- REC uses Blackmagic CCU `Media 10 / Transport mode 1`, mode 2=Record and 0=Preview.
- OSD uses MSPv2 `MSP2_SET_TEXT (0x3007)` and Custom Message type 7–10.
- If OSD does not appear, first use **Send OSD test**. If the test fails, diagnose MSP/Betaflight before BLE.
- If web REC works but AUX does not, diagnose MSP RC/channel mapping.
- If camera pairing works but commands fail, clear bonding on both the camera and FPVCineCam32 and re-pair.

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

`docs/index.html` uses ESP Web Tools. The included GitHub Actions workflow builds the firmware and copies binaries into `docs/firmware/`. Host `docs/` over HTTPS (GitHub Pages is ideal), then open the installer in Chrome/Edge on macOS/Windows/Linux/Android.

Web Serial does **not** work from iOS Safari, so use a Mac/PC for browser flashing.

## License

MIT. Blackmagic Design and Betaflight are trademarks/projects of their respective owners. This project is independent and uses publicly documented protocols.

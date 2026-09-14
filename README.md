# FPVCineCam32 v0.11 DEV - multi-camera / coexistence branch

**v0.10 remains the known-good stable flight build.** v0.11 is intended for the second ESP32-C3 development board.

## What changed

- Camera-first startup: a saved camera gets a clean BLE reconnect window before setup Wi-Fi starts.
- Safe Wi-Fi fallback: setup AP still appears after 4 seconds if the camera is off or fails to reconnect; unconfigured boards expose Wi-Fi after ~600 ms.
- Setup Wi-Fi auto-off remains 90 seconds and returns every reboot.
- Web polling reduced to 2 seconds; MSP OSD text update reduced to 750 ms.
- Blackmagic timecode subscription is disabled because we do not use it; this removes high-rate BLE traffic while setup Wi-Fi is active.
- Explicit camera scans are short and only run on demand; saved cameras reconnect directly without a scan.
- Camera backend selector: Blackmagic / GoPro / DJI Action. Saving a new backend clears the old camera address and restarts into that backend.
- Blackmagic proven REC/STOP path retained. Incoming control subscription now supports notification **or indication** and no longer gates the working outgoing control path.
- Blackmagic media-left decoder is reintroduced conservatively. Unknown data remains `MEDIA --` rather than inventing a value.
- GoPro experimental backend uses official Open GoPro BLE discovery (FEA6), pairing/bonding, GP-0072 command writes and GP-0073 responses. REC/STOP uses Set Shutter packets.
- DJI Action experimental backend discovers official R SDK GATT transport (FFF0 / FFF4 / FFF5). Full DJI R SDK authentication/frame protocol is **not yet enabled**, so DJI REC/STOP is intentionally blocked rather than faked.

## Current feature status

| Feature | Blackmagic | GoPro | DJI Action |
|---|---|---|---|
| Scan/connect | Proven | Experimental | Experimental transport |
| Persistent reconnect | Proven | Experimental | Experimental |
| TX16S REC/STOP | Proven | Experimental | Not yet - R SDK auth required |
| DJI OSD REC/STBY | Proven | Optimistic after successful write | Not yet |
| Media remaining | Experimental | Not implemented | Not implemented |

## Wiring

ESP32-C3 SuperMini hardware profile remains fixed:

- FC TX -> GPIO6 (ESP RX)
- FC RX -> GPIO7 (ESP TX)
- GND -> GND
- 5V -> 5V
- Betaflight UART: MSP 115200

Wi-Fi: `FPVCineCam32-XXXX` / password `fpvcinecam32` / `192.168.4.1`

## Important

Do not replace the working v0.10 board with this until the new branch is proven. This branch intentionally contains experimental camera backends and media telemetry work.

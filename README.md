# FPVCineCam32 v0.10.3 PACKET CAPTURE

Clean diagnostic build based directly on the validated **v0.10.2 MEDIA CAPTURE** baseline.

## Preserved unchanged
- v0.10.2 Wi-Fi startup/order/timing and 90-second idle shutdown
- Blackmagic BLE pairing/reconnect and Incoming Control indication subscription
- TX16S -> Betaflight MSP -> REC/STOP
- DJI OSD REC/STBY
- MSP polling/timing
- No media decoder guess, no timecode fix, no multicamera/GoPro/DJI backend code

## Only functional addition
Diagnostics now keeps a bounded 16-slot capture of incoming Blackmagic CCU packet signatures. A signature is command/category/parameter/data-type/operation. Each slot stores its count, last-seen sequence number, and latest raw payload (max 48 bytes). This prevents fast-changing telemetry from flooding memory while preserving the packet families we need to compare before/after REC.

`incomingCapture` is added to `/api/status`. `mediaRemaining` intentionally remains `--`.

## Repository hygiene
The existing source whitelist remains in `platformio.ini`; only the five intended `.cpp` files compile.

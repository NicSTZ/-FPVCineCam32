# FPVCineCam32 v0.9 ISO test

This build is intentionally based on the exact known-good v0.9 source/runtime.

## What changed

- Custom Message 1: `REC` / `STBY`
- Custom Message 2: `ISO 400`, `ISO 800`, etc.
- If ISO has not been received yet, Message 2 shows `ISO --`.
- No media-left decoder.
- No BLE diagnostic ring buffers.
- No Wi-Fi/startup changes from stable v0.9.
- Web header says `v0.9 ISO` so the test build is easy to identify.

Blackmagic CCU defines ISO as Video group 1, parameter 14, int32. This build only adds that small decode on the existing v0.9 incoming-control parser path.

## OSD

Place two consecutive Betaflight Custom Message elements. If the configured slot is 0:
- Custom Message 1 = REC/STBY
- Custom Message 2 = ISO

`Send OSD test` sends `REC TEST` and `ISO TEST`.

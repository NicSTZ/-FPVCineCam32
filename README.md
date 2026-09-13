# FPVCineCam32 v0.9.8

Blackmagic Pocket Cinema Camera 4K + Betaflight MSP development firmware.

## v0.9.8 — Incoming Control subscription diagnostic

This build stays on the known-good v0.9 runtime/startup path and keeps the v0.9.7 fixed-size raw BLE ring buffer. It adds observation-only diagnostics for the Blackmagic BLE characteristics so we can determine why Incoming Camera Control notifications are not reaching the decoder.

The Diagnostics page now reports:

- Outgoing Camera Control characteristic found
- Incoming Camera Control characteristic found
- Incoming characteristic `canNotify()`
- whether Incoming subscription was attempted and whether it returned success
- Incoming notification count
- Timecode characteristic found / notify capability / subscription result / notification count
- Camera Status characteristic found / notify capability / subscription result / notification count
- Protocol Version characteristic found
- raw Incoming Camera Control bytes if any notifications arrive

REC/STOP, MSP RC, Custom Message OSD and the Wi-Fi startup path are otherwise unchanged. No new media decoder is enabled in this diagnostic build.

## OSD

- Custom Message 1: REC / STBY
- Custom Message 2: media remaining is still under investigation

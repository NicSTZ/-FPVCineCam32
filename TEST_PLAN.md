# FPVCineCam32 v0.9.3 test plan

## 1. Regression

- Flash v0.9.3.
- Confirm the remembered BMPCC 4K reconnects.
- Confirm the camera badge turns green.
- Confirm the TX16S record switch still starts/stops the camera.

## 2. Record-state OSD

Place the selected Custom Message slot in the Betaflight OSD.

Expected:

- Camera disconnected: `CAM OFFLINE`
- Camera connected and idle: `STBY`
- Camera recording: `REC`

Flip REC/STOP repeatedly. The message should change immediately with the camera command.

## 3. Second OSD message

Place the next Custom Message slot in Betaflight OSD.

Press **Send OSD test**. Expected:

- first slot: `REC TEST`
- second slot: `MEDIA TEST`

Normal operation currently shows `MEDIA --` in the second slot. v0.9.3 deliberately disables the media-remaining decoder and keeps raw BLE packet diagnostics. This build is specifically to confirm whether the v0.9.1 media decoder caused the Wi-Fi/startup regression.

## 4. Camera reconnect

Power-cycle the BMPCC without rebooting the ESP. Confirm:

- auto reconnect works
- badge returns green
- OSD returns to `STBY`
- physical record switch still works after reconnect

## 5. Diagnostic capture for media remaining

Open the web diagnostics and note `lastIncoming` while:

1. camera is idle
2. camera starts recording
3. media remaining changes on the camera display

This raw packet snapshot is for finishing the remaining-record-time parser without guessing.

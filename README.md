# FPVCineCam32 v0.10.4 ISOLATED CAPTURE

Clean diagnostic build based directly on the validated **v0.10.3 PACKET CAPTURE** build.

## Preserved unchanged
- v0.10.3 Wi-Fi startup/order/timing and 90-second idle shutdown
- Blackmagic BLE pairing/reconnect and Incoming Control indication subscription
- TX16S -> Betaflight MSP -> REC/STOP
- DJI OSD REC/STBY
- MSP polling/timing
- Existing bounded 16-slot CCU packet capture
- No media decoder guess, no timecode fix, no multicamera/GoPro/DJI backend code

## Only functional addition
A **Clear Capture** button and `/api/clearCapture` endpoint were added to Diagnostics. The web request only sets a reset flag; the capture array is actually cleared on the next incoming Blackmagic packet inside the same BLE callback context that normally updates it. This avoids cross-task mutation of the capture buffer.

Clearing resets only the diagnostic packet groups, sequence numbers and raw last-packet snapshot. It does **not** reset BLE, pairing, camera control, MSP, OSD or the lifetime incoming-packet counter. `mediaRemaining` intentionally remains `--`.

## Repository hygiene
The source tree remains clean and the `platformio.ini` whitelist still compiles only the five intended `.cpp` files.

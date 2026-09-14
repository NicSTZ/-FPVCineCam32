# FPVCineCam32 v0.10.5 9:2 TRACE

Focused diagnostic build based directly on the validated v0.10.4 ISOLATED CAPTURE baseline.

## Preserved unchanged

- v0.10.4 Wi-Fi startup/order/timing and 90-second idle shutdown
- Blackmagic BLE pairing/reconnect and incoming Control indication subscription
- TX16S -> Betaflight MSP -> REC/STOP
- DJI OSD REC/STBY
- MSP polling/timing
- No media decoder guess, no timecode fix, no multicamera/GoPro/DJI backend code

## Only diagnostic change

The broad packet-family capture used in v0.10.3/v0.10.4 is removed from this branch. `incomingCapture` now contains only CCU Change Configuration **category 9 / parameter 2** events. Each event records arrival sequence, `millis()` timestamp, and the exact payload bytes. No meaning is assigned to those bytes yet.

The trace is fixed at 40 entries and wraps in-place, so memory use is bounded. **Clear 9:2 Trace** resets only this diagnostic trace.

`mediaRemaining` intentionally remains `--`.

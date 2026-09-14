# FPVCineCam32 v0.10.6 MEDIA REMAINING

Clean feature build based directly on the validated v0.10.5 branch.

## Preserved unchanged
- Proven v0.10.x Wi-Fi startup/order/timing and 90-second idle shutdown
- Blackmagic BLE pairing/reconnect and Incoming Control indication subscription
- TX16S -> Betaflight MSP -> REC/STOP
- DJI OSD REC/STBY
- MSP polling/timing
- Fixed ESP32-C3 UART profile: FC TX -> GPIO6, FC RX -> GPIO7, 115200

## Functional change
The temporary v0.10.5 category 9 / parameter 2 trace has been removed and replaced with the proven decoder.

Pocket 4K firmware 8.1 reports remaining record duration in CCU category 9 / parameter 2, data type 2, operation 2. The first two payload bytes are little-endian seconds. Hardware captures matched the time shown by the camera.

Examples observed during validation:
- `BA 0D` = 0x0DBA = 3514 s = `58:34`
- `BD 05` = 0x05BD = 1469 s = `24:29`
- `7C 03` = 0x037C = 892 s = `14:52`

`mediaRemaining` now contains the formatted value and the next Betaflight Custom Message slot sends `LEFT <time>`. Values below one hour use `MM:SS`; one hour or more uses `H:MM:SS`.

## Removed as redundant
- v0.10.5 40-entry 9:2 trace ring buffer
- trace timestamps/sequence counters
- Clear 9:2 Trace web control and API endpoint
- broad/old packet-capture experiments

The source whitelist remains in `platformio.ini` so stale files that may still exist in GitHub cannot compile into this build.

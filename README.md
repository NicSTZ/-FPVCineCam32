# FPVCineCam32 v0.10.2 MEDIA CAPTURE

Clean single-purpose build based directly on the validated **v0.10.1 DIAG FIXED** baseline.

## Preserved unchanged
- SoftAP-first startup and 750 ms Wi-Fi head start
- 90-second Wi-Fi idle shutdown and Disable Wi-Fi button
- Blackmagic pairing/reconnect behavior
- TX16S -> Betaflight MSP -> REC/STOP
- DJI OSD REC/STBY
- v0.10.1 serial diagnostics
- No timecode fix, no media decoder guess, no multicamera code

## Only functional change
The Blackmagic **Incoming Camera Control** characteristic is now treated as an isolated telemetry path. The firmware tries Notify first and, only if that is unavailable/fails, Indicate. Failure on this telemetry subscription cannot disable the proven outgoing REC/STOP control path.

The Diagnostics JSON adds:
- `incomingSubscription` (`notify`, `indicate`, `failed`, or `none`)
- `incomingPackets`
- `lastIncoming` (existing raw hex snapshot)

This build deliberately leaves `mediaRemaining` as `--`. We will decode media remaining only after the Pocket 4K firmware 8.1 gives us a real incoming payload.

## Repository hygiene
`platformio.ini` whitelists only the five intended `.cpp` translation units. Old experimental GoPro/DJI `.cpp` files left in the GitHub repository cannot compile into this firmware. This package itself contains no GoPro/DJI source files.

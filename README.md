# FPVCineCam32 v0.10.8 ACTIVE SLOT TRACE

Temporary diagnostic branch based directly on the proven v0.10.7 media-slot trace build.

## Purpose
Capture Blackmagic CCU category **10**, parameter **1** while switching the active recording medium. This is intended to identify the active-media flag/slot mapping without disturbing the already-proven media-remaining decoder.

## Deliberate changes from v0.10.7
- Focused 40-entry diagnostic trace changed from **9:2** to **10:1**.
- Diagnostics button/text changed to **Clear 10:1 Trace**.
- Version banner changed to **v0.10.8 ACTIVE SLOT TRACE**.

## Intentionally unchanged
- Existing 9:2 media-remaining decoder and `MEDIA HH:MM:SS` OSD.
- BLE pairing/reconnect and BMPCC REC/STBY control.
- MSP/RC handling and assigned channel behavior.
- Wi-Fi behavior, GPIO6 RX / GPIO7 TX, baud rate and PlatformIO configuration.

Known-good fallback remains **v0.10.6 MEDIA REMAINING**.

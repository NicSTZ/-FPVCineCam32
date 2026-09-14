# FPVCineCam32 v0.10.7 MEDIA SLOT TRACE

Diagnostic branch based directly on the known-good v0.10.6 MEDIA REMAINING build.

## Purpose
Map BMPCC media slot 1 / 2 / 3 behavior without disturbing the proven control path.

## Changes from v0.10.6
- Restored the focused 40-entry CCU category 9 / parameter 2 trace from v0.10.5.
- Added **Clear 9:2 Trace** to Diagnostics.
- Existing category 9 / parameter 2 decoder remains active.
- Media remaining is displayed as fixed-width `HH:MM:SS`.
- OSD label changed from `LEFT <time>` to `MEDIA <time>`.
- Version banner changed to v0.10.7 MEDIA SLOT TRACE.

## Intentionally unchanged
- BLE pairing / reconnect behavior
- BMPCC REC / STBY control
- MSP transport and RC polling
- RC channel configuration
- Wi-Fi behavior
- GPIO hardware mapping

This is a temporary diagnostic build. v0.10.6 remains the known-good fallback.

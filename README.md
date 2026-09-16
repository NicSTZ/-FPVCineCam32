# FPVCineCam32 v0.10.10 ACTIVE MEDIA FIX

Targeted correction on top of v0.10.9.

## Change

- Corrects the 10:1 receive decoder to accept camera-originated operation `2` telemetry as well as operation `0` control-state packets.
- Keeps the proven Pocket 4K active-media mapping: flag `0x20` = slot 1, `0x40` = slot 2, and captured Pocket 4K extension `0x10` = slot 3 / USB.
- Active slot selects the matching decoded 9:2 remaining-time value. A zero active-media mask clears the display to `MEDIA --`.

## Deliberately unchanged

BLE connection/pairing, REC/STOP writes, Wi-Fi behavior, MSP/OSD, RC mapping, GPIO6/7, and 9:2 media-time decoding are unchanged from v0.10.9.

Known-good fallback remains **v0.10.6 MEDIA REMAINING**.

## OSD display

- The status custom message shows `REC` / `STBY` during normal camera operation.
- The media custom message shows `MEDIA HH:MM:SS`, reflecting the active camera media. If active-media remaining time is unavailable, it shows `MEDIA --`.
- Existing custom-message slots and transmission behavior are unchanged.
- Custom text works on supported DJI systems; user testing confirms O3/O4 with Goggles V2. Do not rely on arbitrary custom text on Vista/original Air Unit with Goggles V1/V2.

This explanation belongs in documentation, not in the live setup GUI.

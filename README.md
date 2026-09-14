# FPVCineCam32 v0.10.9 ACTIVE MEDIA

Targeted production candidate based directly on the proven v0.10.x Blackmagic path.

## Deliberate change
- Decode Pocket 4K category **9 / parameter 2** as three little-endian remaining-time slots.
- Decode category **10 / parameter 1** active-media flags and select the matching 9:2 slot for `MEDIA HH:MM:SS`.
- Blackmagic documents slot 1 active as flag `0x20` and slot 2 as `0x40`; Pocket 4K bench capture established slot 3 / USB as `0x10`.
- A 10:1 packet with no active-media flag clears the OSD to `MEDIA --`, preventing stale time after media removal.
- Before the first 10:1 update, exactly one populated 9:2 slot may be displayed because it is unambiguous. Multiple populated slots are not guessed.
- Diagnostics expose `activeMediaSlot`, `mediaSlots`, `mediaRemaining`, and generic `lastIncoming`. Temporary trace buffers/buttons are removed.

## Intentionally unchanged
- Proven Blackmagic NimBLE pairing/reconnect and REC/STOP write path.
- TX16S switch mapping and MSP polling.
- DJI OSD REC/STBY path and custom-message placement.
- Setup Wi-Fi behavior.
- ESP32-C3 SuperMini GPIO6 RX / GPIO7 TX at 115200.

Known-good fallback remains **v0.10.6 MEDIA REMAINING**.

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

## GoPro connection milestone

Temporary GUI label: **GOPRO CONNECTION TEST**. Version text remains v0.10.10 ACTIVE MEDIA FIX. Rollback: camera-selector build `ca7cee3`.

Select GoPro, Save & Restart, then put the camera in Connect Device / GoPro Quik App mode for the first pairing. Scan, select its advertised name and Connect. Control ready requires an encrypted stored bond, all three response subscriptions, and a successful read-only Get Hardware Info response. No shutter/REC/STOP, camera settings, media or camera Wi-Fi commands are implemented. No keep-alive is sent; camera sleep policy is unchanged.

GoPro target and resolved identity use their own NVS namespace (`fpvcam-gopro`); NimBLE stores encryption/identity keys in its existing persistent bond store. GoPro mode refuses automatic bond eviction if storage fills. Forget GoPro pairing deletes only the saved GoPro peer's bond and GoPro target keys. Blackmagic files and saved target keys are untouched. Connection failures retry at five-second intervals; security/discovery/subscription/readiness failures stop automatic retries for that attempt and appear in the log.

Copy log retrieves the last 64 GoPro connection events from RAM; reboot clears them. No Blackmagic logging was added. Test the requested round trip: GoPro scan/pair/Control ready → ESP restart and bonded reconnect → switch back to Blackmagic and confirm its retained pairing. Software checks cannot establish physical GoPro compatibility.

### Protocol references

- [Official BLE setup and UUIDs](https://gopro.github.io/OpenGoPro/docs/ble/protocol/ble_setup/): FEA6 service; GP-0072/73 Command/Response, GP-0074/75 Settings/Response, GP-0076/77 Query/Response. Only Hardware Info is written to Command.
- [Official query reference](https://gopro.github.io/OpenGoPro/docs/ble/query/#get-hardware-info) and [packet framing](https://gopro.github.io/OpenGoPro/docs/ble/protocol/data_protocol/) define the readiness probe and response handling.
- Behavioral cross-checks: [ESP32-C6 GoPro remote's reported legacy bonding behavior](https://github.com/smillier/GoPro_ESP32C6_Remote), [GoControl scan/subscription behavior](https://github.com/sdebby/GoControl), and [NimBLE GoPro device-name observation](https://github.com/h2zero/NimBLE-Arduino/issues/659). No third-party implementation source was copied.

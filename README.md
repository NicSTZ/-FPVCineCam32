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

Select GoPro, Save & Restart, then put the camera in Connect Device / GoPro Quik App mode for the first pairing. Scan for another GoPro, select its advertised name and Add camera. Control ready requires an encrypted stored bond, all three response subscriptions, and a successful read-only Get Hardware Info response. No shutter/REC/STOP, camera settings, media or camera Wi-Fi commands are implemented. No keep-alive is sent; camera sleep policy is unchanged.

GoPro target and resolved identity use their own NVS namespace (`fpvcam-gopro`); NimBLE stores encryption/identity keys in its existing persistent bond store. GoPro mode refuses automatic bond eviction if storage fills. Each saved card’s Forget action deletes only that GoPro’s bond and card; target keys are cleared only when forgetting the current target. Blackmagic files and saved target keys are untouched. Connection failures retry at five-second intervals; security/discovery/subscription/readiness failures stop automatic retries for that attempt and appear in the log.

Copy log retrieves the last 64 GoPro connection events from RAM; reboot clears them. No Blackmagic logging was added. Test the requested round trip: GoPro scan/pair/Control ready → ESP restart and bonded reconnect → switch back to Blackmagic and confirm its retained pairing. Software checks cannot establish physical GoPro compatibility.

### Protocol references

- [Official BLE setup and UUIDs](https://gopro.github.io/OpenGoPro/docs/ble/protocol/ble_setup/): FEA6 service; GP-0072/73 Command/Response, GP-0074/75 Settings/Response, GP-0076/77 Query/Response. Only Hardware Info is written to Command.
- [Official query reference](https://gopro.github.io/OpenGoPro/docs/ble/query/#get-hardware-info) and [packet framing](https://gopro.github.io/OpenGoPro/docs/ble/protocol/data_protocol/) define the readiness probe and response handling.
- Behavioral cross-checks: [ESP32-C6 GoPro remote's reported legacy bonding behavior](https://github.com/smillier/GoPro_ESP32C6_Remote), [GoControl scan/subscription behavior](https://github.com/sdebby/GoControl), and [NimBLE GoPro device-name observation](https://github.com/h2zero/NimBLE-Arduino/issues/659). No third-party implementation source was copied.

## Saved GoPro cards

Rollback for this UI/state build: hardware-proven GoPro connection build `ffd2183`.

The versioned `cards_v1` NVS blob in `fpvcam-gopro` holds up to eight records: resolved BLE identity/address type, last advertised address/type, camera-reported name, and a friendly name (up to 48 UTF-8 bytes). Names never identify a device. Existing target keys and NimBLE bond storage remain separate and unchanged. The existing NimBLE limit of three total bonds, including Blackmagic, still applies; no automatic eviction was added.

A previously bonded single GoPro migrates automatically. Older firmware did not save its advertised name: the card shows “Camera name not captured” until a later scan supplies it. No extra BLE command is issued to retrieve a name. Saved cards survive offline periods, reboot and camera-type switches. Connected cards expand with bonded/encrypted/control status; REC/STOP placeholders remain disabled. Rename is persistent. Scan is for adding cameras, and resolved identities prevent duplicate saved entries. Only one GoPro connects at a time; Connect on another card deliberately disconnects the current GoPro first.

Short hardware check (flash without erasing):

1. With the already-paired GoPro on, verify its card appears and reaches Control ready without scanning. Rename it, restart ESP, and reload the page; verify its name and bonded reconnect.
2. Turn GoPro off: its card must remain disconnected. Turn it on and confirm recovery. Switch to Blackmagic and back; check BMPCC using the established additional restart if needed, then confirm the GoPro card/name and reconnect survive.
3. Forget that GoPro, verify the card disappears, then scan/add it and confirm first pairing reaches Control ready. If a second GoPro is available, forget its offline card while the first is connected; only that card/bond should disappear.

Host simulations and source comparisons cover state/storage and unchanged protected code; these do not replace the physical checks above.

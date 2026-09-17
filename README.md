# FPVCineCam32 v0.12.0

**FPVCineCam32 v0.12.0 — Proven Blackmagic + GoPro Baseline**

Release preparation based on hardware-proven source `d64e1e8aaf6ea261cef452a3534f52255d580293` and published build `de69a0b`. This version changes release text/documentation only. The v0.12.0 package awaits Nic’s final hardware sanity check; no Git tag or GitHub release has been created.

## Hardware-verified capabilities

- **Blackmagic:** pairing/reconnect; TX16S REC/STOP; authoritative REC/STBY with no false REC without media; active-media detection; remaining record time; DJI/Betaflight OSD.
- **GoPro:** secure pairing/bonding and persistent reconnect; multiple saved GoPros with one active reconnect target; targeted Forget; TX16S and Web UI REC/STOP; camera-reported REC/STBY, battery percentage and remaining video time; DJI/Betaflight OSD; switching active GoPros and switching between GoPro and Blackmagic.
- **System:** shared Betaflight RC mapping; GPIO6 RX / GPIO7 TX MSP at 115200 baud; camera system selector; setup Wi-Fi with the existing 90-second idle behavior; GoPro Copy log and internal `/api/status` diagnostics retained. GoPro time is `2h:28` in the Web UI and `2H:28` in OSD.

The known occasional Blackmagic extra restart after camera-system switching remains unchanged. Custom-text OSD requires a supported DJI system; do not rely on it on Vista/original Air Unit with Goggles V1/V2.

## GoPro power / reconnect

FPVCineCam32 reconnects automatically when the selected GoPro is powered on and available over Bluetooth. If the camera has fully powered down and is no longer advertising over BLE, power the GoPro on normally before use. FPVCineCam32 does not implement BLE wake/power-on.

## Release package

The installer displays **Firmware v0.12.0**. Flash without erasing to preserve existing bonds and configuration. Rollback before this release preparation remains `de69a0b`; the earlier Blackmagic-only v0.10.10 release is preserved. v0.11.x is intentionally skipped because it was associated with abandoned development work.

Final sanity check: confirm the installer version, retained GoPro reconnect plus one TX16S REC/STOP cycle and OSD, then switch to Blackmagic and repeat REC/STOP with media remaining visible.

---

<details>
<summary>Historical milestone notes (retained; not current release instructions)</summary>

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

## GoPro state + OSD test (current development build)

Rollback: hardware-proven REC/STOP build `c2784fe`. This extends the earlier connection/card milestones above; manual REC/STOP remains available, and state/OSD awaits physical validation.

Only three official statuses are registered through GP-0076: Encoding **10 / 0x0A** (one-byte boolean), Internal Battery Percentage **70 / 0x46** (one byte, 0–100), and Remaining Video Time **35 / 0x23** (four-byte big-endian seconds). Request `04 53 0A 46 23` registers these values. GP-0077 response `0x53` supplies initial values; notifications `0x93` supply changes. Registration is repeated once per BLE connection after the existing Hardware Info readiness check. There is no periodic status polling or bitrate-based time estimate.

Recording is unknown, standby or recording, based exclusively on Encoding; shutter acknowledgements never set it. Test in video mode: GoPro defines Encoding as capture activity, and remaining time according to current camera settings. No mode query/change is added. Battery outside 0–100 is unknown. Remaining time is stored in seconds and displayed as whole minutes rounded down (less than a minute displays `0 min`). Disconnect/switch clears the values. Failed, malformed, or timed-out status registration leaves unknown values and a diagnostic error without changing the working BLE reconnect or shutter paths.

### GoPro OSD mapping

Use Betaflight Configurator **Custom Message 1** and **Custom Message 2**:

- **Custom Message 1**, internal slot **0**: `CAM OFF`, `CAM CONNECT`, `CAM READY`, `CAM REC`, or `CAM ERROR`. READY requires control readiness and authoritative standby; unknown/error state is never presented as READY. Registration pending is CONNECT; a failed status path is ERROR.
- **Custom Message 2**, internal slot **1**: `BAT 82% SD 47m`, with unknowns shown as `BAT -- SD --` (each field can be independently unknown).

The combined message fits the existing 31-character limit. Both use the unchanged MSP custom-text writer and existing 500 ms refresh; no alternating text, extra UART transport or GoPro RC mapping. Blackmagic retains its existing OSD formatting and configured slots. Supported DJI custom-message compatibility remains as documented above.

References: [official status IDs](https://gopro.github.io/OpenGoPro/docs/ble/statuses/), [registration and change notifications](https://gopro.github.io/OpenGoPro/docs/ble/query/#register-for-status-value-updates), [packet/TLV format](https://gopro.github.io/OpenGoPro/docs/ble/protocol/data_protocol/), and [official SDK value types](https://github.com/gopro/OpenGoPro/blob/main/demos/python/sdk_wireless_camera_control/open_gopro/api/ble_statuses.py). No third-party source copied.

## TX16 GoPro + truthful Blackmagic REC test (current build)

Rollback: `050e667`. The existing RC channel, threshold, polarity and 300 ms minimum command interval now drive the selected backend. Blackmagic calls its existing `setRecording`; GoPro calls its existing active-ID-checked `setShutter`. A switch edge or newly ready control link applies the position once. Switching saved GoPros reapplies the position to the new active camera. No continuous correction against telemetry is performed: a physical camera shutter change remains authoritative until the mapped switch changes again.

GoPro camera-reported seconds are unchanged. Web UI and Custom Message 2 now use `hours = seconds / 3600`, `minutes = (seconds % 3600) / 60`, formatted `Xh:XX`: 8929 → `2h:28`, 7140 → `1h:59`, 3599 → `0h:59`. Example OSD: `BAT 36% SD 2h:28`; unknown fields remain `--`.

Blackmagic outgoing REC/STOP write success no longer sets recording or its REC/ready label. The unchanged Incoming Camera Control decoder uses Media category 10, Transport Mode parameter 1, int8 `value[0]`: mode 2 is Record; Preview/Play are not recording. Existing operation 0/2 acceptance and all remaining-time/active-media decoding remain unchanged. No-media REC commands are still sent; only camera telemetry may assert REC. [Official Blackmagic Camera Control protocol](https://documents.blackmagicdesign.com/DeveloperManuals/BlackmagicCameraControl.pdf).

Hardware check: use the mapped TX16 switch on each backend, verify a ready/reconnected GoPro receives the initial switch position once, compare `Xh:XX` in UI/goggles, then test Blackmagic REC with no media (must remain STBY) and with media (REC/STOP follows actual camera state). Flash without erasing to retain bonds/settings. This build remains software-validated until those physical checks pass.

</details>

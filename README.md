# FPVCineCam32 v0.10.11-r1 BASELINE UI + LOGS

Development candidate: select Blackmagic or GoPro in setup Wi-Fi, save, and restart.
Only the selected backend is constructed at boot. Blackmagic remains the default
for existing installations and unknown stored selections.

Restored from v0.10.11 (`5197f0e`) with the v0.10.12 bounded connection log and copy button. The v0.10.13 failed-attempt retry change is removed.

The setup page now says Choose camera, explains reboot/reconnect/reload, shows MSP wiring in a table, and displays status/media previews without an editable OSD slot control. Existing saved OSD slots are retained.

## Scope

- Blackmagic retains the v0.10.10 BLE, REC/STOP and active-media implementation.
- GoPro is a selectable placeholder: no BLE, connection, recording or telemetry yet.
- Shared RC mapping, GPIO6 RX / GPIO7 TX at 115200, MSP custom text and 90-second
  setup Wi-Fi behavior are preserved.
- Switching is rejected while the camera reports recording or requests a PIN.
- Selection is stored separately from camera targets. Existing Blackmagic
  `camaddr`/`camtype` keys are retained; GoPro reserves `gpaddr`/`gptype`.
- The old v0.11 GoPro/DJI experimental drivers remain excluded from the build.

## Rollback

Exact original v0.10.11, including firmware: `5197f0eceda69d6d3f017dbdf5175a8e1fc1b366`.
Pre-restoration v0.10.13 remains in history at `f4ebd9712aca6836303237cfbc06e3db72a35e65`.
Documented known-good fallback, including firmware: v0.10.6 at `6b29b3a`.
Reflash without erasing NVS to retain the saved Blackmagic pairing/settings.
Old firmware ignores the new camera-selection key and boots Blackmagic.

## Display compatibility

User testing confirms custom text on O3/O4 with Goggles V2. Do not rely on
arbitrary custom text on Vista/original Air Unit with Goggles V1/V2; normal OSD
working without custom text does not demonstrate a broken ESP-to-FC MSP link.

See TEST_PLAN.md for the short hardware regression procedure.

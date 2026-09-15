# FPVCineCam32 v0.10.12 CONNECTION DIAGNOSTICS

Development candidate: select Blackmagic or GoPro in setup Wi-Fi, save, and restart.
Only the selected backend is constructed at boot. Blackmagic remains the default
for existing installations and unknown stored selections.

## Connection diagnostics

Targeted instrumentation of the v0.10.11 Blackmagic connection path. Pairing
behavior, timeouts, retry scheduling and scan/connect concurrency are unchanged.
This is a diagnostic candidate, not a claimed fix for intermittent connection.

Diagnostics offers Refresh connection log and Copy connection log. The read-only
`/api/connectionLog` endpoint returns firmware version, camera selection, the latest
64 timestamped events, overwritten-event count and the existing status snapshot.
Events persist across attempts/disconnects in the current boot, but not reboot.
No PIN digits, bond secrets, packet streams or NVS writes are logged. Each event
is limited to 127 text bytes. The ring occupies about 8.7 KB RAM; snapshots use
additional temporary heap only when explicitly requested. Log writes and snapshots
use a short critical section; formatting, allocation and HTTP work occur outside it.
The status snapshot is read after the event snapshot, not an atomic camera-state capture.

Copy uses the browser clipboard where permitted, then HTTP-compatible selection/copy,
with manual-copy instructions if the browser refuses. It never reports a stale
log as freshly copied when the request fails. Existing periodic status polling is unchanged.
Numeric NimBLE last-error values are observations; they may reflect an earlier
operation, so they must be interpreted alongside the explicit result and event order.

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

Immediate rollback: v0.10.11 at `5197f0eceda69d6d3f017dbdf5175a8e1fc1b366`.
Earlier candidate, including firmware: v0.10.10 at `17011be8a2d28f4a8f1a85cec4b720ee9f7c2aa2`.
Documented known-good fallback, including firmware: v0.10.6 at `6b29b3a`.
Reflash without erasing NVS to retain the saved Blackmagic pairing/settings.
Old firmware ignores the new camera-selection key and boots Blackmagic.

## Display compatibility

User testing confirms custom text on O3/O4 with Goggles V2. Do not rely on
arbitrary custom text on Vista/original Air Unit with Goggles V1/V2; normal OSD
working without custom text does not demonstrate a broken ESP-to-FC MSP link.

See TEST_PLAN.md for the short hardware regression procedure.

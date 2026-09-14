# v0.10.5 test plan

Use the normal real-flight chain: drone + goggles + TX16S + BMPCC4K + ESP32.

1. Validate normal Wi-Fi, saved BLE reconnect, MSP, REC/STOP and OSD first.
2. Open Diagnostics and tap **Clear 9:2 Trace**.
3. Leave camera in STBY for ~10 seconds; screenshot `incomingCapture`.
4. Tap **Clear 9:2 Trace**.
5. Start REC from TX16S and record for ~20-30 seconds. While still recording, screenshot `incomingCapture`.
6. STOP. Tap **Clear 9:2 Trace**.
7. Leave camera in STBY for ~10 seconds; screenshot `incomingCapture`.

Expected: `mediaRemaining` remains `--`; this build is evidence capture only.

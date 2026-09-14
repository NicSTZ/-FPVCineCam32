# v0.10.4 isolated-capture test

Use the normal flight chain: drone + goggles + TX16S + BMPCC4K + ESP32, with recording media installed.

1. Cold boot and confirm normal Wi-Fi, BMPCC reconnect, TX16S REC/STOP and OSD.
2. In STBY, open Diagnostics and tap **Clear Capture**. Wait ~10 seconds. Screenshot the full `incomingCapture`.
3. Tap **Clear Capture** again. Start REC from TX16S and record ~20 seconds. While still recording, screenshot the full `incomingCapture`.
4. STOP from TX16S. Tap **Clear Capture** again. Wait ~10 seconds in STBY. Screenshot the full `incomingCapture`.
5. Confirm MSP still shows zero timeouts/invalid frames and MEDIA remains `--`.

Do not change codec, frame rate, media or camera settings during this test.

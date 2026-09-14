# v0.10.3 test

Use the normal flight chain: drone + goggles + TX16S + BMPCC4K + ESP32, with recording media installed.

1. Cold boot everything and let the BMPCC reconnect automatically.
2. Confirm Wi-Fi appears normally and REC/STOP still works from the TX16S switch.
3. While camera is STBY, open Diagnostics and capture the full `incomingCapture` field.
4. Start REC from TX16S, record 10-15 seconds, STOP from TX16S.
5. Refresh Diagnostics and capture the full `incomingCapture` field again.
6. Also confirm Custom OSD still shows REC/STBY and MEDIA --.

Do not change camera codec/media/settings during this test. The only controlled variable is REC -> STOP.

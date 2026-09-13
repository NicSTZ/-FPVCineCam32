# v0.9.9 quick test

1. Flash and power normally.
2. Give the AP 10–20 seconds; connect to `FPVCineCam32-XXXX`.
3. Confirm the BMPCC reconnects and REC/STBY works from the TX16S.
4. Confirm Custom Message 1 changes REC/STBY.
5. Confirm Custom Message 2: expected `LEFT ...` if media telemetry arrives, otherwise `MEDIA --`.
6. Leave it powered a few minutes and check Wi-Fi can still be reopened.

This is intentionally the last lean media attempt; no heavy diagnostics are included.

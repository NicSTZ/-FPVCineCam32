# FPVCineCam32 v0.9.5 diagnostic test

1. Flash v0.9.5 and confirm `FPVCineCam32-XXXX` appears normally.
2. Connect to Wi-Fi and open `192.168.4.1`. Confirm the page says v0.9.5.
3. Confirm REC/STBY still works from the TX16S switch.
4. Find **Blackmagic CCU diagnostics** on the page.
5. With media inserted, leave camera in STBY for a few seconds.
6. Start REC, wait a few seconds, then STOP.
7. If convenient, change codec or recording quality so the camera's own remaining-time display changes significantly.
8. Screenshot/copy the diagnostic lines, especially any `CAT 09 PARAM 02` entries and anything that changes between STBY/REC/codec changes.

Expected: Wi-Fi and REC/STBY remain stable. `MEDIA --` may remain until we decode the real packet format from these logs.

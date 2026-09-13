# FPVCineCam32 v0.9.6 raw BLE diagnostic test

1. Flash v0.9.6.
2. Power normally and verify `FPVCineCam32-XXXX` appears.
3. Connect to the AP and open `192.168.4.1`.
4. Confirm the page says v0.9.6 and REC/STBY still works.
5. Scroll to **Raw Blackmagic BLE diagnostics**.
6. Leave the camera in STBY for a few seconds.
7. Start REC, wait a few seconds, then STOP.
8. Change codec or quality so the camera's displayed record-time remaining changes significantly.
9. Take a screenshot of the raw diagnostic block or copy its lines.

Expected diagnostic format:
`#123 LEN 12 DATA FF 05 00 00 ...`

If it says `No raw Incoming Camera Control notifications yet`, send that result too; that tells us the Incoming Camera Control characteristic itself is not notifying.

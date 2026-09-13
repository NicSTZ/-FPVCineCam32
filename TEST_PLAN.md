# FPVCineCam32 v0.9.4 test

1. Flash v0.9.4.
2. Power normally and verify `FPVCineCam32-XXXX` appears.
3. Connect to the AP and open `192.168.4.1`.
4. Confirm the page says v0.9.4.
5. Confirm camera reconnects and REC/STBY still follows the RC switch.
6. Confirm Custom Message 1 switches REC/STBY.
7. Check Custom Message 2 for `LEFT ...`.
8. If media still shows `MEDIA --`, copy the Diagnostics `lastIncoming` value before changing anything else.
9. Power-cycle camera once and verify camera reconnect, REC/STBY, and media text recover.

# FPVCineCam32 v0.9 ISO test plan

1. Flash the build and confirm `FPVCineCam32-XXXX` Wi-Fi appears and remains stable.
2. Open `192.168.4.1`; header should say `v0.9 ISO`.
3. Confirm the BMPCC reconnects normally.
4. Confirm Custom Message 1 still switches `STBY` / `REC`.
5. Custom Message 2 should show `ISO <value>` if the BMPCC sends the ISO CCU update; otherwise it will remain `ISO --`.
6. Change ISO on the BMPCC (for example 400 -> 800) and see whether Message 2 updates.

This is deliberately a lean placeholder build. It does not include media-left decoding or the later diagnostic additions.

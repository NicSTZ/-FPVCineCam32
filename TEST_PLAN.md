# FPVCineCam32 v0.10 test plan

1. Flash over USB with LiPo disconnected.
2. Power-cycle normally.
3. Confirm `FPVCineCam32-XXXX` appears promptly on phone and Mac if available.
4. Connect to `192.168.4.1` and confirm the configurator loads.
5. Confirm saved BMPCC reconnects and TX16S REC/STOP still works.
6. Confirm DJI OSD still shows REC/STBY.
7. Press **Disable Wi-Fi now** and confirm camera control + OSD continue.
8. Reboot and confirm Wi-Fi returns.
9. Final timer test: reboot and do not join the AP. After ~90 s it should disappear while camera control remains working.
10. Repeat several cold boots to judge AP discovery consistency.

# v0.10.1 diagnostic test

Goal: capture one good boot and, ideally, one slow/bad Wi-Fi boot while changing nothing else.

1. Flash normally.
2. Cold boot the normal FPV setup.
3. Observe whether `FPVCineCam32-XXXX` appears normally.
4. If practical, capture the USB Serial log at 115200 for the first ~30 seconds.
5. Confirm BMPCC reconnect + TX16S REC/STOP + REC/STBY OSD still behave exactly like v0.10.

Key lines are `WIFI: softAP() returned ...`, `STATE after AP`, `STATE AP +750ms`, BLE state changes, and the 5-second periodic STATE lines.

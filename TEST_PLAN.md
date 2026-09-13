# FPVCineCam32 v0.9.8 test plan

1. Flash v0.9.8 and power the ESP normally.
2. Confirm `FPVCineCam32-XXXX` Wi-Fi appears and stays up.
3. Connect the BMPCC4K as usual.
4. Open `192.168.4.1` and scroll to **Blackmagic BLE subscription status**.
5. Send a screenshot showing:
   - Incoming Control: found / canNotify / subscribe result / notification count
   - Timecode line
   - Camera Status line
6. Do STBY -> REC -> STOP and press Refresh.
7. If raw Incoming packets appear, include the Raw Blackmagic BLE section in the screenshot.

Do not change codec or other camera settings until the subscription result is known.

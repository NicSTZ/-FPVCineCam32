# v0.10.6 test plan

Use the normal validation sequence.

1. Flash over USB.
2. USB-power ESP32, confirm Wi-Fi appears quickly.
3. Open `192.168.4.1`, power BMPCC4K, confirm BLE connection/pairing and web REC/STOP.
4. Remove USB power.
5. Power the drone from LiPo and confirm saved-camera BLE reconnect, Wi-Fi, TX16S REC/STOP and OSD.
6. Compare the BMPCC4K remaining-time display with Diagnostics `mediaRemaining`.
7. Confirm the second OSD Custom Message shows `LEFT <same time>`.
8. Record for 15-30 seconds and confirm the value decreases and remains consistent with the camera.
9. Change BRAW quality (for example Q1 <-> Q5) and confirm the value updates to the camera's new remaining-time estimate.
10. Confirm MSP remains at 0 timeouts / 0 invalid frames.

Do not change Wi-Fi/BLE/MSP behavior during this validation.

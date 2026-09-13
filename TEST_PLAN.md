# FPVCineCam32 v0.9.2 test plan

1. Upload the extracted v0.9.2 folder contents to the GitHub repo and wait for both Build and Pages to go green.
2. Open the installer and verify it explicitly says **Firmware v0.9.2** before flashing.
3. Flash with the quad LiPo disconnected; power the ESP32-C3 from USB.
4. After reboot, look for `FPVCineCam32-XXXX` immediately. Do not repeatedly reboot it; if first AP startup fails, v0.9.2 retries automatically.
5. Join the AP with password `fpvcinecam32` and open `192.168.4.1`.
6. Verify the page header says **FPVCineCam32 v0.9.2**.
7. Power the normal system and confirm the Blackmagic camera reconnects and the camera badge turns green.
8. Confirm the Betaflight panel still shows MSP connected and live RC channels.
9. Confirm Custom Message 1 changes `STBY` <-> `REC` with the mapped switch.
10. Confirm Custom Message 2 shows `LEFT <time>` if remaining-media data is received; otherwise capture the Diagnostics block for the next targeted media fix.

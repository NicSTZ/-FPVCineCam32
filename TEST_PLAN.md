# FPVCineCam32 v0.11 DEV test plan

Use the **second ESP32-C3 board**. Keep the v0.10 board untouched.

## 1. Blackmagic regression first
1. Select Blackmagic, save/restart.
2. Cold boot with BMPCC already on.
3. Confirm BLE reconnect occurs before/around setup Wi-Fi appearing.
4. Confirm TX16S REC/STOP works and goggles show REC/STBY.
5. Repeat 5 cold boots. Note how long the AP takes to appear on iPhone and Mac.
6. Leave Wi-Fi alone for 90 s and confirm camera control continues after AP shuts down.
7. Confirm there is no timecode dependency: REC/STBY must keep working exactly as before.

## 2. Media left
1. With an SD/CFast/USB media device mounted, change codec/quality so remaining time clearly changes.
2. Watch Custom Message 2 and web `mediaRemaining`.
3. Success = `LEFT 42m` / similar sensible value that changes with codec/media.
4. If it remains `MEDIA --`, capture the Diagnostics block; do not assume a value.

## 3. GoPro experimental
1. Select GoPro -> Save & Restart.
2. Put GoPro into pairing mode.
3. Scan, connect, allow BLE pairing.
4. Test web REC/STOP first, then TX16S switch.
5. This backend is experimental; retain serial/web diagnostics if a command fails.

## 4. DJI Action transport
1. Select DJI Action -> Save & Restart.
2. Enable Wireless Connection on a supported Action camera.
3. Scan/connect.
4. Success for this version = FFF0 service found, FFF4 notify subscribed, FFF5 write characteristic found, status `DJI RSDK LINK - AUTH PENDING`.
5. REC/STOP is intentionally not enabled until the DJI R SDK connection/auth frame layer is ported.

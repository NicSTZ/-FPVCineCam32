# v0.10.11 CAMERA SELECTION — focused hardware check

Use an existing paired Pocket 4K and keep the RC record switch at STOP when
switching camera types. Flash without erasing settings. No trace clearing needed.

1. Boot: confirm Blackmagic is selected and the saved camera reconnects without
   pairing again. Confirm live RC values, saved mapping and OSD slots are retained.
2. Select GoPro, Save and restart. Rejoin setup Wi-Fi and reload. Confirm GoPro is
   selected, diagnostics say GOPRO NOT IMPLEMENTED, and camera controls are disabled.
   Live RC/MSP should still work; OSD should show CAM OFFLINE / MEDIA -- on supported gear.
3. Select Blackmagic and restart. Confirm saved-camera reconnection. Use the TX16S
   switch for one REC -> STOP cycle; confirm camera action and REC/STBY in goggles.
   While recording, the camera-switch button should be disabled.
4. Confirm displayed MEDIA time matches the active camera media; change active
   media once and check it follows. Remove media and check MEDIA --.
5. Disable Wi-Fi using the UI, then repeat one RC REC -> STOP cycle. Reboot once
   without joining the AP: confirm it disappears after about 90 seconds and camera
   control/OSD continue. Reboot to restore setup access.

Report only the failing step and its observed behavior. These steps require real
hardware; compilation does not establish pairing, RF coexistence or OSD rendering.

Rollback: v0.10.10 firmware from commit 17011be, or known-good v0.10.6 from 6b29b3a.
Do not erase NVS during rollback if retaining settings and bonds is desired.

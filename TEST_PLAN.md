# v0.10.13 AUTO RECONNECT — one recovery test

Flash from the FPVCineCam32 page without erasing settings. Do not clear pairing.
Keep the TX16S record switch at STOP.

1. With the camera on, boot the ESP and confirm normal saved-camera reconnection.
2. Leave the ESP powered and connected to setup Wi-Fi. Turn only the camera off
   for 20 seconds so at least one automatic attempt can time out.
3. Turn the camera on with Bluetooth enabled. Do not press Scan or Connect.
   Allow up to 30 seconds for reconnection. It should not require another PIN.
4. Copy connection log before rebooting. Look for RECONNECT retry scheduled after
   failed automatic attempt, then a later successful connection. Paste the log here.
5. Once connected, do one TX16S REC -> STOP cycle. Confirm REC/STBY and MEDIA display,
   live MSP channels and responsive setup Wi-Fi. Disable Wi-Fi and repeat REC -> STOP.

If it does not recover, copy the log before attempting a manual connection or reboot.
No multi-camera, media-slot or timecode investigation is part of this test.

Rollback: v0.10.12 at e8024f9. Known-good fallback: v0.10.6 at 6b29b3a.
Preserve NVS when reflashing to retain settings/bonds.

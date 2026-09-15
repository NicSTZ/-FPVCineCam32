# v0.10.11-r1 BASELINE UI + LOGS — focused check

Flash without erasing settings to check the existing saved pairing. Keep the TX16S record switch at STOP during boot and camera switching.

1. Join setup Wi-Fi. Confirm v0.10.11-r1, quick Wi-Fi access, and saved Blackmagic connection. Copy the connection log before rebooting if it fails; do not clear pairing immediately.
2. Confirm live RC channels and saved mapping. Use the TX16S switch for one REC -> STOP cycle; check camera action, REC/STBY and MEDIA time on supported goggles. Compare remaining time with the camera's active media.
3. Choose GoPro, Save and reboot. Follow the reconnect/reload guidance. Confirm GoPro remains a placeholder and MSP still responds. Choose Blackmagic again; confirm the saved connection returns.
4. Disable Wi-Fi and repeat one RC REC -> STOP cycle. Reboot to restore setup Wi-Fi. The unchanged 90-second no-client shutdown can be checked on a later unattended boot.

Use Copy connection log if any step fails and report the step. Fresh pairing and repeated power-cycle stress tests are not required for this first baseline check. Hardware behavior remains unverified until tested on the device.

Rollback: original v0.10.11 source and firmware at `5197f0e`; known-good v0.10.6 at `6b29b3a`. Retain NVS when preserving bonds/settings.

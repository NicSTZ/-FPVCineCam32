# FPVCineCam32 v0.9.6

Diagnostic build based directly on the stable v0.9.4 / v0.9 startup path.

Changes in this build:
- keeps the proven Wi-Fi, BLE reconnect, REC/STBY and MSP Custom Message paths unchanged
- keeps the existing experimental media-remaining decoder
- captures the last 20 notifications directly from Blackmagic **Incoming Camera Control** BLE characteristic
- raw capture happens before any CCU packet parsing or interpretation
- shows notification length and up to the first 48 bytes as hex in the webpage
- Custom Message 1 remains REC / STBY
- Custom Message 2 remains LEFT ... when/if media remaining decodes
- webpage and installer labels are v0.9.6

The Wi-Fi AP remains always on in this development build.

# FPVCineCam32 v0.10.1 DIAG

Diagnostic build based directly on the known-good v0.10 Wi-Fi coexistence build.

## Purpose
Determine what the ESP32-C3 is actually doing during the intermittent setup-Wi-Fi startup without redesigning startup or camera control.

## Runtime behavior
Intentionally unchanged from v0.10:
- same SoftAP-first startup
- same 750 ms Wi-Fi head start
- same Blackmagic BLE pairing/reconnect logic
- same Betaflight MSP/RC handling
- same REC/STOP behavior
- same REC/STBY OSD behavior
- same 90-second idle Wi-Fi shutdown
- same Disable Wi-Fi button

## Only functional addition
USB Serial diagnostics at 115200 baud. Logs boot timing, SoftAP return value/IP/mode, BLE/control state transitions, station count and free heap. A compact state line is also printed every 5 seconds.

This build deliberately does **not** add media decoding, ISO, multicamera support or any BLE protocol changes.

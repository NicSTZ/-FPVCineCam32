# FPVCineCam32 v0.9.9 — clean media build

Built directly from the exact known-good v0.9 source.

Changes are deliberately small:
- keeps the proven TX16S -> Crossfire -> Betaflight MSP -> ESP32 -> Blackmagic REC/STOP path
- Custom Message 1 remains REC / STBY
- Custom Message 2 is again media remaining (`LEFT 42m`, `LEFT 1h24m`) or `MEDIA --` if the camera does not publish it
- adds only the Pocket-camera remaining-record-time decoder; no ISO decoder and no raw diagnostic ring
- BLE callback only copies primitive media values; text formatting happens in the normal loop
- web page refresh reduced from 500 ms to 1500 ms to reduce Wi-Fi/BLE radio contention on the ESP32-C3
- Wi-Fi startup sequence is otherwise unchanged from v0.9

Hardware profile remains GPIO6 RX / GPIO7 TX / MSP 115200.

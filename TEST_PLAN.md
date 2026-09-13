# FPVCineCam32 v0.3 bench test plan

Do these in order. Do not troubleshoot multiple layers at once.

## 1 — ESP only
- Flash and boot from USB.
- Confirm `FPVCineCam32-XXXX` Wi-Fi exists.
- Join with password `fpvcinecam32` and open `192.168.4.1`.

## 2 — MSP only (camera can stay off)
- Wire GND, FC TX→ESP RX, FC RX→ESP TX.
- Enable MSP 115200 on the spare Betaflight UART.
- Web status should show MSP connected and API `1.47` on BF 2025.12.
- Place Custom Message 0 in the OSD.
- Press **Send OSD test**. Goggles should show `BMD LINK TEST`.
- Move the chosen AUX switch and confirm its channel is correct before involving the camera.

## 3 — Blackmagic pairing only
- Enable Bluetooth on BMPCC 4K.
- Scan from FPVCineCam32.
- Select the camera.
- Camera should show a six-digit PIN.
- Web page should change to `ENTER 6-DIGIT PIN`.
- Submit PIN. Camera should become paired/ready.
- Power-cycle both devices. They should reconnect without re-entering the PIN.

## 4 — Camera control
- Use web **REC** and **STOP** first.
- Confirm camera's physical red record indication changes.
- Confirm web state changes from incoming camera notifications, not only from the command sent.

## 5 — Full chain
- Toggle the configured TX AUX channel.
- FC MSP RC → ESP → BMD BLE should start/stop record.
- OSD should show `REC <timecode>` while recording and `BMD STBY <timecode>` otherwise.

## Fault isolation
- OSD test fails: UART/MSP/Betaflight problem; ignore BLE.
- OSD works, web REC fails: BLE/pairing/BMD packet problem.
- Web REC works, AUX fails: MSP RC mapping/threshold problem.
- REC command works but state is wrong: incoming BMD transport parser/notification problem.
- Pairing PIN never appears: encrypted Camera Status write did not initiate bonding; inspect serial logs and NimBLE security callback.
- Random BLE dropouts in flight: disable setup Wi-Fi (automatic after 90 s) and retest before changing BLE logic.

# FPVCineCam32 v0.7 bench test plan

Test one layer at a time.

## 1. Camera regression check

- Power the ESP32 and BMPCC 4K.
- Join `FPVCineCam32-XXXX` and open `192.168.4.1`.
- Confirm the remembered camera reconnects without asking for the PIN again.
- Press **REC test** and **STOP test**.
- Confirm the camera actually records/stops and the web state follows it.

## 2. Wire Betaflight MSP

ESP32-C3 SuperMini v0.7 uses fixed pins:

- FC TX -> ESP GPIO6 (RX)
- FC RX -> ESP GPIO7 (TX)
- GND -> GND
- MSP baud -> 115200

In Betaflight Ports, enable MSP on that spare UART only.

## 3. Confirm MSP link

The web Betaflight panel should show:

- `MSP connected`
- API version (expected `1.47` on the current BF 2025.12 setup)
- live CH1-CH16 values
- increasing response counter
- low/zero timeouts
- zero invalid frames

Move sticks/switches and verify the corresponding channel values change.

## 4. Configure REC/STOP mapping

- Select the desired RC channel. Nic's current default is CH11.
- Default threshold is 1500.
- Choose **Above threshold** or **Below threshold** for REC.
- Save mapping.
- The selected channel's live value is shown beside the mapping.

## 5. Full control test

With BMPCC connected and control-ready:

- Move the selected switch into REC state.
- Camera should start recording.
- Move it back into STOP state.
- Camera should stop recording.
- Repeat at least 10 times.
- Power-cycle the ESP32 and repeat without re-pairing.

## 6. Diagnostics

Healthy target:

- RC responses steadily increase.
- Invalid frames stay at 0.
- Timeouts remain 0 or very low.
- Last RC response time remains low and stable.

Fault isolation:

- No MSP connection/API: check UART selection, crossed TX/RX wiring, common ground, and MSP 115200 in Betaflight.
- MSP connected but no channel movement: verify receiver channels in Betaflight Receiver tab and MSP_RC responses.
- Channel moves but camera does not respond: verify mapping threshold/direction and camera `controlReady`.
- Web REC works but RC control does not: problem is MSP/mapping, not Blackmagic BLE.
- RC control works: proceed to camera-state -> MSP2 Custom Message -> DJI OSD testing.

## 7. Wi-Fi note

v0.7 intentionally keeps Wi-Fi on indefinitely for bench testing. Restore automatic Wi-Fi shutdown only after MSP and OSD are proven.

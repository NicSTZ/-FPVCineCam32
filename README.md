# FPVCineCam32 v0.9.2

FPVCineCam32 bridges Betaflight MSP RC channels to a Blackmagic Pocket Cinema Camera 4K over BLE and sends camera status back to Betaflight Custom Messages for DJI OSD.

## v0.9.2 changes

- Targeted Wi-Fi startup reliability fix; no intentional changes to the working REC/STBY or media-remaining decoder.
- Starts the setup Wi-Fi AP **before** MSP and BLE camera initialization.
- Retries AP startup internally up to three times.
- If the AP still fails at boot, retries it every 5 seconds in the background instead of requiring repeated power cycles.
- Disables Wi-Fi sleep while the development AP is running.
- Delays BLE auto-reconnect until after the AP has had time to establish.
- Version is now consistent everywhere: installer, manifest and controller webpage all say **v0.9.2**.
- Custom Message selected slot remains `REC` / `STBY`.
- Next Custom Message slot remains `LEFT <time>` when the BMPCC4K reports remaining record time.

## Hardware profile

ESP32-C3 SuperMini UART is fixed:

- FC TX -> GPIO6 (ESP RX)
- FC RX -> GPIO7 (ESP TX)
- FC GND -> ESP GND
- MSP 115200

## OSD setup

If the selected slot is Custom Message 1, place both Custom Message 1 and Custom Message 2 in the Betaflight OSD layout.

- Message 1 = camera record state
- Message 2 = media remaining

`Send OSD test` sends `REC TEST` and `MEDIA TEST`.

## Browser flasher

The GitHub Pages installer uses ESP Web Tools and the binaries produced by the existing GitHub Actions workflow.

## License

MIT. Blackmagic Design and Betaflight are trademarks/projects of their respective owners. This project is independent and uses publicly documented protocols.

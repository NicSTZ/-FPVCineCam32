# FPVCineCam32 v0.9.7

FPVCineCam32 bridges Betaflight MSP RC channels to a Blackmagic Pocket Cinema Camera 4K over BLE, and sends camera status back to Betaflight Custom Messages for DJI OSD.

## v0.9.7 changes

- Keeps the proven v0.8 TX16S -> Crossfire -> Betaflight -> MSP -> ESP32 -> BMPCC REC/STOP control path.
- Removes timecode from the flight OSD.
- Custom Message selected slot: `REC`, `STBY`, `CAM OFFLINE`, `CAM ENTER PIN`, or `CAM WAIT`.
- Next Custom Message slot: reserved for media remaining. In this build it displays `MEDIA --` until the Pocket 4K media-remaining BLE payload is decoded.
- REC/STOP state now updates immediately after a successful control write, so the OSD follows the physical switch without waiting for a camera echo.
- Incoming Blackmagic CCU Transport Mode packets are still parsed and can correct the state when the camera reports them.
- Adds `lastIncoming` raw BLE packet diagnostics to the web status JSON to help finish the media-remaining decoder.
- Green/red Camera connected badge retained.
- Development Wi-Fi remains on continuously for bench testing.

## Hardware profile

ESP32-C3 SuperMini UART is fixed:

- FC TX -> GPIO6 (ESP RX)
- FC RX -> GPIO7 (ESP TX)
- FC GND -> ESP GND
- MSP 115200

## OSD setup

If the selected slot is Custom Message 1, place both Custom Message 1 and Custom Message 2 in the Betaflight OSD layout.

- Message 1 = camera record state
- Message 2 = media remaining (decoder work in progress in v0.9.7)

`Send OSD test` sends `REC TEST` and `MEDIA TEST` to prove both elements are visible.

## Browser flasher

The GitHub Pages installer uses ESP Web Tools and the binaries produced by the existing GitHub Actions workflow.

## License

MIT. Blackmagic Design and Betaflight are trademarks/projects of their respective owners. This project is independent and uses publicly documented protocols.

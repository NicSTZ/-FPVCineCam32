# FPVCineCam32 v0.9.5

Diagnostic build based directly on the stable v0.9.4 runtime.

## Purpose
- Keep the proven Wi-Fi, BLE reconnect, MSP and REC/STBY paths unchanged.
- Capture the actual Blackmagic CCU Change Configuration packets sent by the BMPCC4K.
- Expose the last 10 decoded CCU commands on the controller webpage as `CAT / PARAM / TYPE / OP / LEN / DATA`.
- Retain the current safe media-left decoder, but do not guess further until the real camera packets are observed.

## Test
Open `192.168.4.1`, then perform STBY, REC, STOP and (ideally) change codec/quality so the camera's displayed remaining record time changes. Copy or screenshot the **Blackmagic CCU diagnostics** block.

Installer, manifest and webpage labels are all v0.9.5.

# v0.10.8 ACTIVE SLOT TRACE — focused test

1. Flash v0.10.8 and confirm the BMPCC reconnects normally.
2. Confirm REC/STBY still works from the assigned RC switch.
3. Confirm the goggles still show `MEDIA HH:MM:SS`.
4. Open Diagnostics and confirm **Clear 10:1 Trace** is present.
5. With both SD and SSD inserted, select **SD (slot 2)** on the camera.
6. Press **Clear 10:1 Trace**.
7. Change only the active recording medium **SD (slot 2) -> SSD (slot 3)**. Do not change codec/quality and do not record.
8. Wait a few seconds, press Refresh, and screenshot `incomingCapture`.
9. If useful, repeat **SSD (slot 3) -> SD (slot 2)** after clearing the trace again.

We are looking only for the exact **10:1 value bytes** that change with active-media selection.

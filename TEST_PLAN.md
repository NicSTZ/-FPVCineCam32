# v0.10.7 MEDIA SLOT TRACE — focused test

1. Flash v0.10.7 and confirm the BMPCC reconnects normally.
2. Confirm REC / STBY still works from the assigned RC switch.
3. Confirm the goggles show `MEDIA HH:MM:SS` and that the value still matches the camera for slot 1.
4. Open Diagnostics and confirm **Clear 9:2 Trace** is present.
5. Set the camera to slot 2.
6. Press **Clear 9:2 Trace**.
7. Switch only to slot 3. Do not change codec / quality and do not start recording.
8. Wait a few seconds, press Refresh, and capture the `incomingCapture` / 9:2 trace.
9. Record the remaining time shown on the camera for slot 3 alongside the trace.

Known reference from the current bench test: slot 3 displayed `01:27:22`.

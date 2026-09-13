# FPVCineCam32 v0.9.4

Built directly from the known-good v0.9 source.

Changes in this build:
- keeps the exact v0.9 startup order and Wi-Fi path
- keeps proven REC/STBY control and MSP Custom Message output
- adds a defensive Blackmagic remaining-record-time decoder
- media packet validation happens in the BLE callback, but String formatting is deferred to the main loop
- Custom Message 1: REC / STBY
- Custom Message 2: LEFT 42m / LEFT 1h24m
- webpage and installer version labels are v0.9.4

The Wi-Fi AP remains always on in this development build.

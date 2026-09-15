# v0.10.12 CONNECTION DIAGNOSTICS — one focused test

Flash from the FPVCineCam32 installer without erasing settings. Keep existing
pairing; do not press Forget pairing. Keep the TX16S recording switch at STOP.

1. Power the camera with Bluetooth enabled. Boot the ESP, join its setup Wi-Fi,
   and open 192.168.4.1. Confirm v0.10.12 and Blackmagic selection.
2. Allow saved auto-connect to finish. If it does not connect, scan and click the
   camera once. Wait about 15 seconds; enter the PIN if the camera requests it.
3. In Diagnostics press Copy connection log, and paste the text into the chat.
   If automatic copy is blocked, the text is selected for manual Copy. Screenshots
   of the log also work. Copy before rebooting, even if the attempt failed.
4. If connected, do one TX16S REC -> STOP cycle and confirm camera action,
   REC/STBY and media remaining. Report whether Wi-Fi setup stayed responsive.

Do not intentionally spam Connect or clear pairing for the first test. If you
naturally need another attempt, copy after the failure first, then repeat normally.
The log records overlapping requests without changing how the firmware handles them.
No repeat of the full multi-camera/media-slot test is required for this diagnostic-only change.

Software checks performed: full ESP32-C3 build; host ring-buffer test covering
wraparound, truncation, concurrent writers and snapshots; web copy tests covering
secure clipboard, HTTP legacy copy, manual fallback and failed fetch; exact
baseline comparison after removing trace calls. Hardware behavior remains unverified.

Immediate rollback: v0.10.11 at 5197f0e. Earlier candidate: v0.10.10 at 17011be.
Known-good fallback: v0.10.6 at 6b29b3a. Preserve NVS when reflashing.

# v0.10.9 ACTIVE MEDIA — focused regression

Keep codec/quality unchanged during this test.

1. Flash v0.10.9 and allow the saved Pocket 4K to reconnect.
2. Confirm TX16S REC/STOP still works and REC/STBY still appears in the goggles.
3. Select each installed medium on the camera and check the media OSD:
   - slot 1: approximately `00:24:18`
   - slot 2: approximately `00:23:37`
   - slot 3 / USB: approximately `01:27:22`
4. Remove all media. The OSD should become `MEDIA --`, not retain the previous value.
5. Only if a slot is wrong, send one Diagnostics screenshot showing `activeMediaSlot`, `mediaSlots`, `mediaRemaining`, and `lastIncoming`.

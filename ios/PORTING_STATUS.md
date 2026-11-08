# iOS port — status and implementation plan

The Android port remains the reference implementation. iOS work is isolated on the `ios-port` branch.

## Implemented on this branch

- Standalone SDL3 iOS host scaffold and unsigned app bundle workflow.
- Landscape app metadata and Files-app document sharing.
- App Documents storage at `On My iPhone/Shaolin Monks`, with an `assets` subfolder.
- Discovery of extracted `default.xbe` and `.iso` files in both supported folders.
- File inspection metadata: file type and size.
- Streaming SHA-1 calculation for an extracted `default.xbe` using Apple's CommonCrypto API; the entire executable is not loaded into memory for hashing.
- Diagnostic UI and logs now show the detected file size and XBE SHA-1 fingerprint.
- Android JNI-only virtual-controller callbacks remain guarded from non-Android builds.

## Current milestone

**Milestone 1 — iOS host and game-data inspection.** The app shell can enumerate supported files and calculate an extracted XBE fingerprint. The fingerprint is not yet compared with the build's expected XBE hash. ISO files are discovered and sized but are not yet opened or mounted by the iOS host.

## Next engineering work

1. Verify the latest macOS iOS-device and simulator builds and fix any compile errors.
2. Connect the XBE fingerprint to the exact build fingerprint generated from the source game's XBE.
3. Port the Android loader's release-selection behavior and Xbox ISO parsing/mounting to the iOS host using the shared SDK runtime.
4. Replace the diagnostic screen with the actual recompiled game runtime.
5. Port or validate the renderer (MoltenVK or a native Metal backend), then wire game audio, lifecycle, touch controls and gamepad input.
6. Make XBE recompilation reproducible on macOS, or generate the C++ game sources in a separate Windows job.
7. Add signing/provisioning and IPA export after the unsigned app builds and launches.

## Important boundary

This is still a porting scaffold, not a playable game or installable signed IPA. No copyrighted game data is included. The user must provide their own legally obtained game files privately through the iOS Files app.

# iOS port — status and implementation plan

The Android port remains the reference implementation. iOS work is isolated on the `ios-port` branch.

## Implemented on this branch

- Standalone SDL3 iOS host scaffold and unsigned app bundle workflow.
- Landscape app metadata and Files-app document sharing.
- App Documents storage at `On My iPhone/Shaolin Monks`, with an `assets` subfolder.
- Discovery of extracted `default.xbe` and `.iso` files in both supported folders.
- File inspection metadata: type, size and streaming SHA-1 for extracted XBE files.
- Optional build-time release fingerprint: CMake accepts `IOS_XBE_IMAGE` (defaults to the repository's private `assets/default.xbe` when present), calculates its SHA-1 and exposes it to the app.
- Extracted XBE files are compared against the expected build fingerprint.
- The shared Xbox disc reader from the SDK is now compiled into the iOS host. Each detected ISO is opened and its Xbox filesystem is inspected for `default.xbe`; the embedded executable is hashed and compared against the expected fingerprint.
- The diagnostic screen reports whether an ISO is recognized, whether it contains the executable, and whether its release matches.
- No copyrighted game data is added to the public repository.

## Current milestone

**Milestone 2 — game-data validation.** The iOS host now reuses the shared SDK Xbox ISO reader to inspect disc images and identify/hash their embedded executable. The current host only inspects the ISO; it does not yet retain/mount the selected disc or boot the recompiled game.

## Next engineering work

1. Verify the latest macOS iOS-device and simulator builds and fix any compile errors.
2. Adapt the game loader to retain the selected `kernel::Disc`, mount it with `kernel::MountDisc`, and provide the validated executable bytes to the runtime.
3. Replace the diagnostic screen with the actual recompiled game runtime.
4. Port or validate the renderer (MoltenVK or a native Metal backend), then wire game audio, lifecycle, touch controls and gamepad input.
5. Make XBE recompilation reproducible on macOS, or generate the C++ game sources in a separate Windows job.
6. Add signing/provisioning and IPA export after the unsigned app builds and launches.

## Important boundary

This is still a porting scaffold, not a playable game or installable signed IPA. No copyrighted game data is included. The user must provide their own legally obtained game files privately through the iOS Files app.

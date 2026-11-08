# iOS port — status and implementation plan

The Android port remains the reference implementation. iOS work is isolated on the `ios-port` branch.

## Implemented on this branch

- Standalone SDL3 iOS host scaffold and unsigned app bundle workflow.
- Landscape app metadata and Files-app document sharing.
- App Documents storage at `On My iPhone/Shaolin Monks`, with an `assets` subfolder.
- Discovery of extracted `default.xbe` and `.iso` files in both supported folders.
- File inspection metadata: type, size and streaming SHA-1 for extracted XBE files.
- Optional build-time release fingerprint: CMake accepts `IOS_XBE_IMAGE` (defaults to the repository's private `assets/default.xbe` when present), calculates its SHA-1 and makes it available to the app.
- Extracted `default.xbe` is now compared against that expected fingerprint; the diagnostic screen reports a match, a different release, or that verification is unavailable.
- No copyrighted game data is added to the public repository.

## Current milestone

**Milestone 1 — iOS host and XBE release inspection.** File discovery, file sizing, SHA-1 calculation and optional expected-release comparison are implemented in the host scaffold. The expected hash is only available when the original build XBE is supplied privately at CMake configure time. ISO files are discovered but are not yet parsed or mounted by the iOS host.

## Next engineering work

1. Verify the latest macOS iOS-device and simulator builds and fix any compile errors.
2. Port the Android loader's ISO release detection and Xbox ISO parsing/mounting to the iOS host using the shared SDK runtime.
3. Replace the diagnostic screen with the actual recompiled game runtime.
4. Port or validate the renderer (MoltenVK or a native Metal backend), then wire game audio, lifecycle, touch controls and gamepad input.
5. Make XBE recompilation reproducible on macOS, or generate the C++ game sources in a separate Windows job.
6. Add signing/provisioning and IPA export after the unsigned app builds and launches.

## Important boundary

This is still a porting scaffold, not a playable game or installable signed IPA. No copyrighted game data is included. The user must provide their own legally obtained game files privately through the iOS Files app.

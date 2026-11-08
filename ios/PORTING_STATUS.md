# iOS port — status and implementation plan

The Android port remains the reference implementation. iOS work is isolated on the `ios-port` branch.

## Verified repository facts

- The game is an original Xbox `default.xbe` statically recompiled to C++.
- The Android host links Vulkan, Android and Android logging libraries.
- Android-specific host files live under `native/`; game/runtime code lives under `src/`.
- SDL3 is already the cross-platform window/input/audio layer.
- The current `build.bat` requires Windows, Visual Studio and the Android NDK; it is not an iOS build pipeline.
- The Android renderer uses Vulkan and Dear ImGui's Vulkan backend. iOS requires a Metal renderer or a fully validated MoltenVK route.

## Implemented on this branch

- `.github/workflows/ios-port-audit.yml`: macOS toolchain/repository audit.
- `.github/workflows/ios-sdl3-smoke.yml`: builds iPhoneOS arm64 and iOS Simulator arm64 targets on a macOS runner, and uploads the unsigned device .app bundle.
- `ios/CMakeLists.txt`: standalone iOS CMake target linking SDL3 static.
- `ios/src/main.cpp`: minimal SDL3 app host with a non-zero fullscreen landscape window, a Metal renderer preference, event loop and renderer diagnostic screen.
- `ios/Info.plist`: app bundle metadata and landscape orientation.
- `ios/README.md`: current architecture and next tasks.

## Current milestone

**Milestone 1 — native SDL3 iOS host scaffold.** The app shell and CI build path are committed. The workflow must run successfully before we can call the scaffold build-verified. The diagnostic screen is not the game.

## Remaining engineering work

1. Run and fix the macOS iOS-device build until it produces the unsigned `.app` artifact.
2. Replace the diagnostic screen with the game's runtime entry point after isolating portable sources from Android host code.
3. Port the renderer and Dear ImGui backend to Metal, or validate MoltenVK end-to-end on iOS.
4. Replace Android storage, activity lifecycle, logging and JNI assumptions with iOS equivalents.
5. Make the XBE recompiler reproducible on macOS, or generate the C++ game sources in a separate Windows job.
6. Implement private/local import of legally obtained game files; never commit copyrighted disc data to this public repository.
7. Add signing/provisioning and IPA export after the unsigned app builds and launches.

## Important boundary

This branch currently contains an iOS shell, not a working port of the game. No game assets are included. An installable signed IPA requires Apple signing and provisioning.

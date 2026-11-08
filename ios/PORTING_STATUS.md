# iOS port — status and implementation plan

This repository is currently the Android port. The iOS port is being developed on the ios-port branch so the Android build remains untouched.

## Verified starting point
- The game executable is an original Xbox default.xbe statically recompiled to C++.
- The existing host is Android-specific: the root CMake target links vulkan, android, and log, and compiles files from native/.
- SDL3 is already the cross-platform window/input/audio layer.
- The Android app expects legally obtained game disc files outside the source repository.

## Required iOS work
1. Separate portable game/runtime sources from the Android host.
2. Add an iOS host target and app entry point using SDL3's iOS integration.
3. Replace Android-only storage, lifecycle, logging, permissions and JNI/NDK assumptions with iOS equivalents.
4. Verify graphics: iOS does not expose Vulkan natively, so use a tested MoltenVK path or port the renderer to Metal.
5. Build the Xbox XBE recompiler and generated game code in a macOS-compatible pipeline. The documented build.bat currently requires Windows/Visual Studio.
6. Add private/local game-data installation. Do not commit copyrighted ISO/disc data to this public repository.
7. Build unsigned first; IPA distribution requires Apple signing and provisioning.

## Current milestone
The first GitHub Actions workflow on this branch audits the repository on a macOS runner and uploads a report. It is an audit, not an IPA build: there is not yet an Xcode project, iOS host, or iOS-compatible recompiler pipeline.

## Definition of done
- iOS app builds on a macOS GitHub Actions runner.
- Game data can be installed without embedding copyrighted assets in the public repository.
- The app launches the game and supports rendering, touch/controller input, audio, saves, pause/resume and orientation.
- A signed IPA is exported only after signing credentials and provisioning are configured.
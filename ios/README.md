# iOS port work area

This folder contains the iOS-specific port plan and build experiments. The Android app remains the reference implementation and is not modified by this work.

## Current target

- Native iPhone/iPad build (arm64, iOS 15.0 minimum for the first smoke test).
- SDL3 for window lifecycle, input and audio.
- The game runtime and generated Xbox XBE code must be made portable before the full app can link.
- Graphics are a separate blocker: the Android host uses Vulkan and the ImGui Vulkan backend. iOS needs a verified Metal path or a deliberately integrated MoltenVK stack.

## First validation

The `iOS SDL3 device build smoke test` workflow checks whether the pinned SDL3 3.2 release branch can be configured and compiled for the iPhoneOS SDK on GitHub's macOS runner. This is a dependency test, not a game build and does not produce an IPA.

Run it from GitHub Actions using **iOS SDL3 device build smoke test → Run workflow**.

## Next engineering tasks

1. Identify and isolate Android-only source files and libraries in the root CMake target.
2. Port the Vulkan renderer and ImGui backend to Metal (or validate MoltenVK end-to-end).
3. Replace Android storage, activity lifecycle, logging and JNI dependencies with iOS equivalents.
4. Make the XBE recompiler/toolchain work on macOS, or generate the C++ game sources in a separate reproducible Windows job and transfer only generated source artifacts.
5. Add an SDL3 iOS app host and an Xcode archive/export pipeline.
6. Add a private game-data import flow. Game disc files must not be committed to this public repository.

## Important

A successful SDL3 smoke test does not mean the game is ported. No game assets are included, and a distributable IPA will require Apple signing/provisioning.

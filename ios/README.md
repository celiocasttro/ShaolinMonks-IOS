# iOS port work area

This folder contains the native iOS host scaffold. The Android app remains the reference implementation and is not modified by this work.

## What exists now

- `CMakeLists.txt` creates an iOS app bundle and links SDL3 as a static library.
- `src/main.cpp` initializes SDL3, opens a fullscreen landscape window, requests SDL3's Metal renderer, creates an SDL renderer and handles app events.
- `Info.plist` declares the iOS app bundle metadata and landscape orientation.
- GitHub Actions checks out SDL3, configures and builds both the iPhoneOS arm64 target and the arm64 iOS Simulator target with Xcode; the unsigned device `.app` is uploaded as a workflow artifact.

The current screen is only a diagnostic host screen. It does not launch Shaolin Monks yet.

## Run the build

The workflow is triggered by commits to `ios/**` on `ios-port`, or manually from GitHub Actions using **iOS native host build → Run workflow**.

Expected artifact: `ShaolinMonksIOS-unsigned-app`.

## Next engineering tasks

1. Confirm the macOS device and simulator builds complete and fix any compiler/linker errors.
2. Identify and isolate Android-only source files and libraries in the root CMake target.
3. Port the Vulkan renderer and Dear ImGui Vulkan backend to Metal, or validate MoltenVK end-to-end.
4. Replace Android storage, lifecycle, logging and JNI dependencies with iOS equivalents.
5. Make XBE recompilation reproducible on macOS, or generate C++ game sources in a separate Windows job.
6. Connect the game runtime to the iOS host and add a private/local game-data import flow.
7. Add Apple signing/provisioning before exporting an installable IPA.

## Important

A successful host build does not mean the game is ported. No game disc data is included in this public repository. The IPA will not be installable on a normal iPhone until an appropriate signing/sideloading route is configured.

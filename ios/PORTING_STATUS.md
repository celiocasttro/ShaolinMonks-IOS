# iOS port — status and implementation plan

The Android port is the reference implementation. All iOS work is isolated on the `ios-port` branch.

## Verified repository facts

- The game is an original Xbox `default.xbe` statically recompiled to C++.
- The Android host's root CMake target links Vulkan, Android and Android logging libraries.
- The host compiles Android-specific files from `native/` and game/runtime files from `src/`.
- SDL3 is the cross-platform window/input/audio layer, but the current Android CMake setup expects Gradle to provide an SDL checkout.
- The documented `build.bat` requires Windows, Visual Studio and the Android NDK; it is not an iOS build pipeline.
- The Android renderer uses Vulkan and Dear ImGui's Vulkan backend. iOS needs a verified Metal solution or a tested MoltenVK integration.

## Work committed on this branch

1. `.github/workflows/ios-port-audit.yml` — inventories the Apple toolchain and current platform blockers on a macOS runner.
2. `.github/workflows/ios-sdl3-smoke.yml` — independently configures and builds SDL3 for the iPhoneOS arm64 SDK.
3. `ios/README.md` — records the iOS target, validation boundary and engineering sequence.

## Current milestone

**Milestone 1: validate the SDL3 iOS dependency.** The new workflow is ready to run in GitHub Actions. Its result must be checked before treating SDL3 as validated for this target.

This is not yet an iOS game host and does not generate an IPA. There is still no Xcode app target, iOS renderer, iOS storage/lifecycle layer, or macOS-compatible XBE recompiler pipeline.

## Remaining implementation

1. Separate portable runtime/game sources from the Android host.
2. Port the renderer and ImGui backend to Metal, or validate MoltenVK end-to-end on iOS.
3. Implement iOS lifecycle, logging, file import and save paths.
4. Make the XBE recompilation process reproducible on macOS, or produce generated C++ in a separate Windows job.
5. Add the SDL3 iOS application target and build/archive workflow.
6. Support private/local installation of legally obtained game files; do not commit copyrighted disc data to this public repository.
7. Configure Apple signing/provisioning before exporting an installable IPA.

## Definition of done

The iOS app builds on a macOS runner, imports the user's game files privately, launches the game with graphics/audio/input/saves working, and exports a signed IPA after signing credentials are configured.

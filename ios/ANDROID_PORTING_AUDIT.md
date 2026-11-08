# Android-to-iOS port audit

This document records source-level findings from the current Android implementation. It is an engineering map, not a claim that the game already runs on iOS.

## Rendering decision: test MoltenVK before rewriting the renderer

The existing renderer is not a generic SDL renderer. `native/vulkan.cpp` implements the Xbox Direct3D 8 GPU path on Vulkan and uses:

- SDL's Vulkan window/surface integration (`SDL3/SDL_vulkan.h`, `SDL_Vulkan_GetInstanceExtensions`, `SDL_Vulkan_CreateSurface`).
- Vulkan swapchains, device/queue selection, image and buffer allocation, render passes, descriptors and pipelines.
- Dynamic rendering when Vulkan 1.3 or `VK_KHR_dynamic_rendering` is available.
- Timeline semaphores when supported, with a fence fallback.
- Dear ImGui's Vulkan backend.
- HLSL translated to SPIR-V by `native/vk_shader.cpp` and glslang.

A direct Metal rewrite would replace a substantial renderer. The lower-risk first experiment is to keep the Vulkan renderer and link the iOS MoltenVK XCFramework, which maps Vulkan to Apple's Metal and translates SPIR-V shaders to Metal Shading Language. MoltenVK documents support for iOS and timeline semaphores; dynamic rendering support must still be checked against the exact MoltenVK version and device capabilities.

This is a proposed route, not yet an implemented or validated dependency. If a required Vulkan feature is missing or too slow on target iPhones, revisit a native Metal backend.

## Source boundaries verified

### Android host: replace, do not compile on iOS

`native/main.cpp` is explicitly Android-specific. It includes Android logging and system-property headers, reads Android device properties, calls SDL Android storage APIs, logs Android SDK information, and sets Android back-button behavior. It also owns the SDL window/event loop, app foreground/background state, storage paths and ImGui platform input.

The iOS target needs a separate host implementation for those responsibilities:
- iOS app lifecycle and foreground/background handling.
- Writable app Documents/Caches directories and game-data import.
- Logging to the Xcode device console.
- SDL window creation with `SDL_WINDOW_VULKAN` when using MoltenVK.
- Touch input and optional controller input.
- Safe pause/resume and renderer surface recreation.

### Renderer: candidate for reuse with platform guards

`native/vulkan.cpp` is the existing renderer. It includes SDL Vulkan and Vulkan headers, and creates its own Vulkan instance and surface. It should remain shared if the iOS build can provide MoltenVK and all required extensions/features. Do not compile the current diagnostic SDL 2D renderer as a substitute for this game renderer.

`native/vk_shader.cpp` compiles HLSL into SPIR-V with glslang. MoltenVK can translate SPIR-V to MSL at runtime, so the shader language itself does not force a rewrite.

### Common/runtime code

The root `CMakeLists.txt` currently builds an Android shared library and explicitly links `vulkan android log`. It also includes `native/*.cpp` wholesale. The iOS build must use an explicit source list and exclude Android host files rather than globbing all native sources.

The XBE recompiler output is expected under `generated/game`, with the original `default.xbe` at `assets/default.xbe`. Neither copyrighted game data nor generated proprietary game content should be added to this public repository. The iOS app needs a private/local import or provisioning process for files the user legally owns.

## Ordered implementation gates

1. Pin SDL3 to the same release as Android (currently `release-3.4.16`) so the two hosts use a consistent SDL API.
2. Build and link MoltenVK's iOS XCFramework in the iPhoneOS target; confirm SDL can create a Vulkan surface on a physical iPhone.
3. Add a capability report for swapchain, dynamic rendering, timeline semaphores, required formats, descriptor limits and presentation modes. Keep the renderer's existing fence fallback.
4. Create an iOS host that replaces Android storage, logging, lifecycle and touch/controller input.
5. Replace the Android CMake source glob with explicit shared/runtime and platform-host source lists.
6. Compile the renderer and Dear ImGui Vulkan backend against MoltenVK; resolve unsupported features based on runtime capability reports.
7. Integrate the XBE-generated code and private game-data import, then test boot, menus, gameplay, audio, suspend/resume and controller/touch controls on a physical iPhone.
8. Add signing and export an IPA only after the app launches and the game runs.

## Current state

The iOS SDL3 shell is a diagnostic app only. The game runtime, MoltenVK framework, Vulkan game renderer, iOS host adapter, touch controls, game-data loader, signing and IPA export are not yet integrated.

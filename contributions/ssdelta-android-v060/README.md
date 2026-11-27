# SS Delta 0.6.0: local Android integration for review

This updates the previously proposed contributions in ss-delta issue #29 onto the official **0.6.0** runtime (`3c2ae20`). It is a community test build, not an upstream release.

## Retained official implementations

The cooperative pause and its maintainer fixes, background pause, performance overlay/FPS counter, PUP picker/size/hash validation/download cache, Vulkan driver loader and selection, and official installation/error handling are retained. They are not proposed again here. No Resident project instrumentation is included.

## Local additions already proposed

- Grouped English dark installer, success indicators, Build/Clear, input locking, installation progress on the setup screen. Native install steps retain their isolated SDL process but use a translucent/invisible installation window. A required on-device module-dump run stays visible.
- ARM64 folder/ZIP import, automatic modules.json, expected-file/ELF64/AArch64/size/hash validation, and no temporary dump-game run when imported modules are used. Without an import, original on-device compilation remains. Reuse of an installed user-imported set retains native ABI/code-hash checks. The game-version CHECK remains before importing a replacement set.
- Existing touch editor: direct selection/drag, size/shape tools, circular D-pad, fixed/floating sticks and local activation regions, stick response settings, add/remove/rebind, independent combinations and toggles, touch-coordinate buttons, geometry/removal fixes and reduced idle update work.
- Existing 24/30/40/45/60 limiter with live switching, and 0.5x/0.75x/1x native 3D modes with restart; HUD/general scale stays separate. One limiter update path; one official FPS counter. Host tests are not proof of correct simulation speed/cadence in every scene.
- Driver ZIP import/storage and picker; actual loading remains official. The older custom startup/crash-recovery guard is NOT active in this APK. Actual crash-recovery validation remains separate work.
- Bounded diagnostic export with a scoped read-only sharing provider.

## Exact tested build and evidence

APK version `0.6.0-local.3`, versionCode 1002, package `io.github.vitarecomps.ssdelta.importfix`. APK SHA-256: `6c4c546644536ac3629d0b4ca8a9fd91521b24717b44ed0c907a6c91b019f5df`.

ELLINDON reports the app working after several tests, and supplied a Mi Pad 5 video recorded with Turnip, native 3D 0.75x, general scale 1x and target 45 FPS. This is tester feedback; no frame-time analysis of the recording, controlled simulation-speed comparison, or claim of a locked measured 45 FPS is made. Other devices/effects/lifecycle/crash paths remain unverified.

The last interface correction removes horizontal-row weight from the official firmware status TextView when moving it into a vertical card, so it wraps content instead of expanding the card. The installer now uses explicit View references, not label matching or child-position discovery. This also avoids the earlier encoding/text-lookup failure.

## Source/build notes

This branch includes the complete tracked official runtime sources plus the integrated local source files. Third-party dependencies remain pinned submodules. No proprietary generated game/firmware source or .so modules, PKG, licence, PUP, signing key or device log is included.

Use the official Android NDK build workflow for the `ss-delta` runtime and the Android frontend Java sources/manifest in this tree. `CMakeLists.txt` also contains a local Windows-host/NDK Boost adaptation; it is build infrastructure, not a runtime optimization or part of the focused feature PRs.

`android-setup.py` is the exact modified build-tools entry point: official 0.6.0 compiler/firmware behavior with the existing ARM-import wrapper. Put it at `kit/tools/android-setup.py` when packing the official build-tools asset and regenerate the archive file-count/SHA metadata. The official 0.6.0 build-tools archive was used, not an earlier tool kit. Users provide their own matching game 1.30, licence, PUPs and/or matching ARM modules. A change requiring regenerated game modules does not rewrite an already imported .so automatically.

This is a source review/integration snapshot; the historical local packaging used apktool/D8 and a private test key. It is not a claim of a portable one-command or bit-for-bit reproduction recipe. Focused PRs will separate changes for upstream review; some remain draft candidates.

Developed with ChatGPT/Codex assistance; the initial work was previously disclosed as Codex 6.1 assisted. Device testing and feedback by ELLINDON. Original project/dependency credits and licences remain with their authors.

SS Delta Android local modifications - source review kit

Prepared for https://github.com/vita-recomps/ss-delta/issues/29
Contributor: ELLINDON. Modifications developed with ChatGPT/Codex 6.1 assistance; device testing and feedback by ELLINDON. Original project/runtime credits and original file licences remain applicable. No claim to ownership of upstream code.

REFERENCE
Stable local APK: ss-delta-driver-recuperacao-teste.apk (not distributed here).
Original adaptation started from v0.5.1. This is a review kit, NOT a release or a drop-in build tree. Historical patches have different local checkpoints and some short paths. They must not be blindly concatenated or assumed to apply to current upstream HEAD. The source snapshots supply context and helper files omitted from historical patches. Port one topic at a time to the target upstream revision. UI resources/manifest wiring and the PC build workflow are not a complete reproducible upstream package here; importer Java is a historical reference, not a replacement for the current installer.

CONTENTS
patches/: focused pause, driver recovery, native-resolution and FPS patches; later editor geometry/performance fixes.
source/: helper headers, affected pause/runtime files and game/frontend hook context. frontend.cpp and settings.cpp are explicitly the snapshots saved BEFORE the rejected PowerVR experiment.
android-controls/: latest local Java controls/editor sources, including combos, touch-coordinate buttons, floating-stick activation regions and reduced redundant update work.
android-importer-reference/: importer diagnostics/provider, native-launch and custom-driver import reference sources from the local adaptation. Integrate and review against current upstream instead of replacing the installer blindly.
tests/: host concurrency, policy, bounds and timing tests, plus synthetic old/new analog regression harness.

TESTING
The pause was confirmed working by the device tester. 24/30/40/45 FPS were reported stable in tested scenes only; this does not guarantee every scene. Lower native 3D modes were tested locally. Driver startup recovery after an actual driver crash still needs device validation. Host tests do not replace Vulkan/device testing.
C++ host tests require C++20. Include source/util/include for pause_tests.cpp (and threads support); source/game-frontend for driver_guard_tests.cpp; source/games/ss-delta for resolution/FPS tests. Run driver_guard_tests with a writable temporary directory as its argument. ControlsCostRegression.java is a standalone synthetic regression harness, not an Android performance benchmark.

LIMITATIONS
Pause is cooperative at HLE/LLE/run-loop boundaries. Wall clocks are not frozen. Audio admission synchronizes producers and may have a performance cost. Resolution patching is game-specific and should retain bounds checks and unchanged retail-mode constants. FPS patching preserves the existing game timing fixes.
The rejected PowerVR per-draw framebuffer synchronization experiment is EXCLUDED. Its APK was deleted at the user's request and it must not be integrated as a validated fix.

EXCLUSIONS
No APK, native shared library, generated game/firmware C/C++, PKG, licence/work.bin, firmware, signing keystore, personal device log or build credentials are included. Runtime/game hook source here is hand-maintained project integration code, not generated game module output.

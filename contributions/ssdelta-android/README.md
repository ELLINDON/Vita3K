# SS Delta Android local test build and source review

Community contribution by ELLINDON, developed with ChatGPT/Codex assistance. Original ss-delta, Vita3K, recompilation tools and dependency credits/licences remain with their authors.

## What is provided

The release APK is the latest local build tested and approved by ELLINDON. **Only Soul Sacrifice Delta 1.30 is supported.** It contains no compiled game/firmware modules, game data, PKG, licence/work.bin, Sony firmware/font PUPs or signing key. Users supply their own files.

- Existing enhanced touch editor: direct selection/dragging, size/shape tools, circular D-pad, floating/fixed sticks, configurable local activation regions, stick response options, add/remove/rebind, button combinations, hold/toggle, touch-coordinate buttons, independent input sources and geometry/removal fixes.
- Existing local pause correction, diagnostics export, optional FPS counter, driver import/recovery candidate, native 3D resolution options and extra FPS targets. Known validation limits below still apply.
- Folder/ZIP import of matching user-built ARM64 `.so` modules. The Android app creates `modules.json`; the user need not create one. It checks expected names, ARM64 ELF shared-library format and SHA-256 integrity. The loader retains code-hash/ABI checks.
- With imported modules, setup skips the temporary module-dump game launch. Without them, **Build** uses the original on-device compilation workflow, including its temporary game run.
- Optional offline PUP import using the project's pinned firmware/font sizes and SHA-256 hashes; partial/invalid offline sets fail without silently downloading. Without offline imports, the original download path remains.
- Grouped dark setup UI, English strings, original Build/Clear actions, verification marks, setup buttons locked while installing, integrated progress on the setup screen, teal Play and landscape orientation. The native extraction remains in its original isolated SDL activity/process; only its installation window is translucent/invisible. Percentages come from existing log entries; otherwise the bar is indeterminate.

## Source layout and reproducibility limits

This branch retains the upstream `games` tree at `1955f5fca3289c5cc90fb41ab3ff78888ca32a68` and adds the local contribution sources under this directory. **It is a complete review collection, not a claim that all changes are already integrated into current upstream or that checking out this branch directly reproduces the APK.**

- `stable-local-snapshots/`: all previously shared hand-maintained changes, patches, touch-editor Java sources, importer references and tests. Its README documents different historical checkpoints; do not concatenate the patches blindly. The rejected PowerVR graphics workaround is excluded.
- `installer/source/`: full editable Java sources for the final importer/UI/progress helpers and modified `android-setup.py`.
- `installer/integration/`: exact final BuildActivity/FrontendStepActivity smali and manifest wiring used by this APK. Original controls/native state is retained from the stable local base.
- `installer/tests/`: host regression checks.
- `installer/build-apk.py` and `verify-apk.py`: local assembly/check scripts. They contain local paths and assume the stable local base; adapt them rather than treating them as a portable upstream build command.

The editable code and exact installer wiring are provided so the maintainer can inspect or reimplement the changes without taking our binary as an upstream-ready patch. Submodules/dependencies remain referenced by the upstream tree. No proprietary generated C/ARM64 modules are included.

The native libraries in the published APK are byte-for-byte the stable local driver-recovery build (12 libraries unchanged by the installer/UI updates). **The APK uses the earlier local pause prototype; the separate draft [pause PR](https://github.com/vita-recomps/Vita3K/pull/1) removes its global audio admission lock and has additional JIT integration work. This APK's approval does not validate that different PR revision.**

## Validation

ELLINDON reported that installation completed successfully, liked the revised setup layout, then approved the final integrated-progress build. Existing controls and pause had also been tested in prior local sessions. This is practical tester feedback, not exhaustive testing of every device, loading/movie/JIT path or driver crash.

Host checks: Java compilation, DEX/APK assembly, signature and 16 KiB alignment; 10 module-package checks, 6 progress-reader checks and 5 setup-flow checks. Device-compilation dispatch was tested with a stub, not by rerunning the expensive compiler. The final integration additionally checks preservation of native libraries, module exclusion, landscape and the existing 1.30 eboot-hash policy.

Known limits remain: PowerVR graphics artifacts; no controlled game-speed/pacing validation for 24/40/45 FPS on 60 Hz; 60 FPS experimental; custom-driver crash recovery and the separate pause PR's full JIT/lifecycle/movie matrix are not comprehensively validated. This test build is not an official release.

## Provenance

Original projects: https://github.com/vita-recomps/ss-delta and https://github.com/vita-recomps/Vita3K . Discussion: https://github.com/vita-recomps/ss-delta/issues/29 . No claim to ownership of upstream code.

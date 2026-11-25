# Cooperative frontend pause: validation notes

This change addresses vita-recomps/ss-delta#29. Frontend pause is a persistent session gate, separate from guest/debugger suspension. Native HLE entry/return, LLE entry and dispatch observe it. A JIT stop still calls HaltExecution through stop(); its normal run loop and nested sentinel loop both check the gate before re-entry. Native tight loops that never reach a cooperative boundary are not preempted. Exit/delete publishes cancellation and notifies waiters; shutdown releases the gate before graphics cleanup and joins.

The proposed revision deliberately leaves SDL audio uploads unchanged. Unlike the first local prototype, it does not hold a global mutex across SDL_PutAudioStreamData. A call already inside the audio HLE may finish one enqueue; the return gate prevents it from starting another guest call until resume. Device audio queue behavior still needs integration testing.

## Host checks

Standalone gate tests: configure this test-only project with `cmake -S vita3k/util/tests -B build-pause-tests`, build it, then `ctest --test-dir build-pause-tests --output-on-failure`. No game, firmware or renderer is required. Cases cover late-waking guests, cancellation/wakeup races, rapid resume races, nested re-entry contract and releasing multiple workers. The nested-loop test models the gate contract, not the Dynarmic implementation.

## Reported device behavior of the earlier local build

ELLINDON tested native gameplay pause/resume on Moto g54 and Mi Pad 5 with no observed deadlock or persistent audio problem; a short, almost imperceptible FPS dip on resume quickly normalized. That earlier build included the audio admission lock removed here. Those reports therefore do not validate this exact PR revision, JIT mode, every movie/loading transition or wall-clock behavior.

## Integration checks still needed

- Native gameplay, loading screens and movies: open/close settings repeatedly; check picture/audio resume and no renderer/GXM deadlock.
- `[recompiled] jit = true`: both an executing JIT and nested native/JIT transitions must pause/resume without changing guest/debugger suspension.
- Exit/relaunch while paused and app lifecycle pause/resume.
- Verify that in-flight audio enqueue without the removed global lock does not cause a persistent mismatch.

Keep this PR draft until the integration matrix is completed. Clocks are not frozen by this change.

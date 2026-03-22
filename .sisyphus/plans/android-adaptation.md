# Android Platform Adaptation for VGLite ThorVG

## TL;DR

> **Quick Summary**: Implement Android EGL backend support for VGLite ThorVG, enabling GPU direct rendering on Android with compile-time platform isolation. Create NativeActivity APK demo showing 5-color cycle.
> 
> **Deliverables**:
> - Platform-aware `vglite_gl_config.h` with `__ANDROID__` detection
> - Android EGL context handling in `vg_lite_tvg.cpp`
> - Android CMake build configuration for arm64-v8a
> - NativeActivity APK demo (installable, renders color cycling)
> 
> **Estimated Effort**: Medium (multi-phase, cross-platform)
> **Parallel Execution**: YES - 3 waves (platform macros, build system, demo)
> **Critical Path**: Platform macros → CMake config → EGL init → Demo render

---

## Context

### Original Request
1. Analyze vg_lite_clear dependencies for minimal rendering
2. Adapt vg_lite API for Android with macro isolation (preserve Windows support)
3. Create Android APK demo based on clear_demo.cpp

### Interview Summary
**Key Discussions**:
- **Platform Target**: Android API 21 (Lollipop), arm64-v8a only
- **Build System**: Gradle + CMake (Android Studio standard)
- **NDK Version**: r27c (user has, compatible with ThorVG's r21e)
- **Demo Type**: NativeActivity (pure C++), 5-color cycling like Windows demo
- **Backend**: GL only for Android (SW backend excluded)

**Research Findings**:
- **ThorVG EGL Support**: Built-in via `THORVG_GL_TARGET_GLES=1` macro
- **EGL Function Loading**: ThorVG loads `libEGL.so` via `dlopen()` automatically
- **OpenGL ES Requirement**: GLES 3.0+ (API 18+ supports, API 21 recommended)
- **Context Handling**: ThorVG's `GlRenderer::currentContext()` has EGL code path

### Metis Review
**Identified Gaps** (addressed):
- **Context Lifecycle**: Android may destroy EGL context on pause → Lock portrait orientation to avoid surface recreation complexity
- **EGL Swap**: `vg_lite_finish()` doesn't call `eglSwapBuffers()` → Demo app controls swap timing
- **Memory Alignment**: Android needs `memalign()` instead of `_aligned_malloc()` → Add platform-specific allocation

---

## Work Objectives

### Core Objective
Enable VGLite ThorVG to render on Android using external EGL context, with complete compile-time isolation from Windows implementation.

### Concrete Deliverables
- `vglite_gl_config.h` — Platform detection macros (`VGLITE_PLATFORM_ANDROID`, `VGLITE_PLATFORM_WINDOWS`)
- `vg_lite_tvg.cpp` — Android EGL context handling (guarded by `__ANDROID__`)
- `examples/android/CMakeLists.txt` — Android NDK CMake configuration
- `examples/android/src/main/cpp/main.cpp` — NativeActivity entry point
- `examples/android/build.gradle` — Gradle build script
- `examples/android/src/main/AndroidManifest.xml` — APK manifest (portrait locked)
- Working APK: `vglite-android-demo.apk` (arm64-v8a)

### Definition of Done
- [ ] Windows SW backend builds and runs (regression test)
- [ ] Windows GL backend builds and runs (regression test)
- [ ] Android APK builds with `./gradlew assembleDebug`
- [ ] APK installs on Android device/emulator (API 21+)
- [ ] APK displays 5-color cycling (Red→Green→Blue→Yellow→Cyan)
- [ ] Logcat shows "VGLITE init success" and "EGL init success"

### Must Have
- Compile-time platform isolation (no Windows code compiled on Android)
- `THORVG_GL_TARGET_GLES=1` defined for Android builds
- Android NativeActivity lifecycle handling (init/term window)
- Portrait orientation locked in manifest

### Must NOT Have (Guardrails)
- NO modification to ThorVG library (libs/thorvg-1.0.2/)
- NO breaking changes to Windows SW/GL backends
- NO SW backend on Android (GL only)
- NO runtime platform detection (compile-time only)
- NO complex features (rotation, multi-window, etc.) — minimal demo only

---

## Verification Strategy (MANDATORY)

> **ZERO HUMAN INTERVENTION** — ALL verification is agent-executed. No exceptions.

### Test Decision
- **Infrastructure exists**: NO (Android APK build is new)
- **Automated tests**: None (visual demo only)
- **Agent-Executed QA**: ALWAYS (mandatory for all tasks)

### QA Policy
Every task MUST include agent-executed QA scenarios.
Evidence saved to `.sisyphus/evidence/task-{N}-{scenario-slug}.{ext}`.

- **Android Build**: Use Bash (cmake/gradlew) — Configure, build, verify APK exists
- **Android Install**: Use Bash (adb) — Install, launch, check logcat
- **Android Visual**: Use Bash (adb screencap) — Capture screenshot, compare color

---

## Execution Strategy

### Parallel Execution Waves

```
Wave 1 (Start Immediately — platform macros + build scaffolding):
├── Task 1: Platform detection macros [quick]
├── Task 2: Android memory allocation [quick]
├── Task 3: Android CMakeLists.txt scaffolding [quick]
├── Task 4: Gradle build configuration [quick]
└── Task 5: AndroidManifest.xml [quick]

Wave 2 (After Wave 1 — EGL and ThorVG integration):
├── Task 6: NativeActivity entry point [unspecified-high]
├── Task 7: EGL initialization/termination [unspecified-high]
├── Task 8: vg_lite Android integration [unspecified-high]
└── Task 9: Clear demo render loop [unspecified-high]

Wave 3 (After Wave 2 — verification and polish):
├── Task 10: Windows regression test [quick]
├── Task 11: Android build verification [quick]
└── Task 12: Android runtime test [unspecified-high]

Critical Path: Task 1 → Task 3 → Task 6 → Task 7 → Task 8 → Task 9 → Task 12
Parallel Speedup: ~60% faster than sequential
Max Concurrent: 5 (Wave 1)
```

### Dependency Matrix

- **1-5**: — — 6-9, 1
- **6**: 3, 4, 5 — 7-9, 1
- **7**: 6 — 8-9, 1
- **8**: 7 — 9, 1
- **9**: 8 — 10-12, 1
- **10**: — — 12, 2
- **11**: 9 — 12, 2
- **12**: 9, 10, 11 — 2

### Agent Dispatch Summary

- **Wave 1**: **5** — T1-T5 → `quick`
- **Wave 2**: **4** — T6-T9 → `unspecified-high`
- **Wave 3**: **3** — T10-T11 → `quick`, T12 → `unspecified-high`

---

## TODOs

- [ ] 1. **Platform Detection Macros**

  **What to do**:
  - Update `vglite_gl_config.h` to detect `__ANDROID__` and define platform macros
  - Add `VGLITE_PLATFORM_ANDROID` and `VGLITE_PLATFORM_WINDOWS` macros
  - Auto-set `VGLITE_USE_GL_BACKEND=1` for Android
  - Ensure Windows behavior unchanged

  **Must NOT do**:
  - Modify ThorVG library files
  - Change Windows SW/GL backend behavior

  **Recommended Agent Profile**:
  - **Category**: `quick`
    - Reason: Single file change, straightforward conditional compilation
  - **Skills**: []
    - None needed for this simple macro addition

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 1 (with Tasks 2-5)
  - **Blocks**: Tasks 6-9 (need platform macros)
  - **Blocked By**: None (can start immediately)

  **References**:
  - `vglite_gl_config.h` - Current platform config, add Android branch

  **Acceptance Criteria**:
  - [ ] `vglite_gl_config.h` has `#if defined(__ANDROID__)` branch
  - [ ] `VGLITE_PLATFORM_ANDROID` defined for Android
  - [ ] Windows build passes (regression test)

  **QA Scenarios**:
  ```
  Scenario: Windows build regression
    Tool: Bash (cmake)
    Preconditions: No changes to Windows code paths
    Steps:
      1. cd examples && cmake -B build_test -DVGLITE_USE_GL_BACKEND=OFF
      2. cmake --build build_test --config Release
    Expected Result: Build succeeds, no errors
    Evidence: .sisyphus/evidence/task-01-windows-build.log
  ```

  **Commit**: YES
  - Message: `feat(android): add platform detection macros`
  - Files: `vglite_gl_config.h`

- [ ] 2. **Android Memory Allocation**

  **What to do**:
  - Add Android-specific aligned memory allocation in `vg_lite_tvg.cpp`
  - Use `memalign()` for Android instead of `_aligned_malloc()` (Windows)
  - Add `#if defined(__ANDROID__)` guard around allocation code
  - Ensure Windows allocation path unchanged

  **Must NOT do**:
  - Change Windows memory allocation logic
  - Add runtime platform detection

  **Recommended Agent Profile**:
  - **Category**: `quick`
    - Reason: Single function modification with conditional compilation
  - **Skills**: []

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 1 (with Tasks 1, 3-5)
  - **Blocks**: Task 8 (needs allocation)
  - **Blocked By**: None

  **References**:
  - `vg_lite_tvg.cpp:vg_lite_allocate()` - Find memory allocation code
  - ThorVG pattern: `libs/thorvg-1.0.2/src/lib/renderer/sw_engine/tvgSwRenderer.cpp` - See how ThorVG handles platform allocation

  **Acceptance Criteria**:
  - [ ] Android uses `memalign()`
  - [ ] Windows uses existing `_aligned_malloc()`
  - [ ] Windows build passes

  **QA Scenarios**:
  ```
  Scenario: Windows allocation unchanged
    Tool: Bash (cmake + grep)
    Steps:
      1. Build Windows SW backend
      2. grep for "_aligned_malloc" in vg_lite_tvg.cpp - should exist in Windows path
    Expected Result: _aligned_malloc still present and used for Windows
    Evidence: .sisyphus/evidence/task-02-alloc-check.txt
  ```

  **Commit**: YES
  - Message: `feat(android): add aligned memory allocation for Android`
  - Files: `vg_lite_tvg.cpp`

- [ ] 3. **Android CMakeLists.txt Scaffolding**

  **What to do**:
  - Create `examples/android/` directory structure
  - Create `examples/android/CMakeLists.txt` for Android NDK
  - Configure for arm64-v8a ABI
  - Define `THORVG_GL_TARGET_GLES=1` compile definition
  - Link required Android libraries: `log`, `android`, `EGL`, `GLESv3`
  - Include ThorVG sources and vglite sources

  **Must NOT do**:
  - Modify existing examples/CMakeLists.txt (Windows)
  - Add unnecessary dependencies

  **Recommended Agent Profile**:
  - **Category**: `quick`
    - Reason: Standard CMake configuration, well-documented pattern
  - **Skills**: []

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 1 (with Tasks 1-2, 4-5)
  - **Blocks**: Tasks 6-9 (need build system)
  - **Blocked By**: None

  **References**:
  - `examples/CMakeLists.txt` - Existing Windows CMake for reference
  - `tests/thorvg/CMakeLists.txt` - ThorVG source file list
  - NDK sample: `ndk/sources/android/native-activity/` - Standard NativeActivity CMake pattern

  **Acceptance Criteria**:
  - [ ] `examples/android/CMakeLists.txt` exists
  - [ ] cmake configure succeeds for Android NDK
  - [ ] THORVG_GL_TARGET_GLES=1 defined

  **QA Scenarios**:
  ```
  Scenario: CMake configure for Android
    Tool: Bash (cmake)
    Preconditions: Android NDK r27c available
    Steps:
      1. cd examples/android
      2. cmake -B build -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a
    Expected Result: CMake configure succeeds without errors
    Evidence: .sisyphus/evidence/task-03-cmake-config.log
  ```

  **Commit**: YES
  - Message: `feat(android): add CMakeLists.txt for Android build`
  - Files: `examples/android/CMakeLists.txt`

- [ ] 4. **Gradle Build Configuration**

  **What to do**:
  - Create `examples/android/build.gradle` (project level)
  - Create `examples/android/app/build.gradle` (module level)
  - Configure for API 21 minimum
  - Configure NDK version r27c
  - Enable CMake build integration
  - Set compile options for C++17

  **Must NOT do**:
  - Use deprecated Gradle features
  - Over-configure (keep minimal)

  **Recommended Agent Profile**:
  - **Category**: `quick`
    - Reason: Standard Android Gradle configuration
  - **Skills**: []

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 1 (with Tasks 1-3, 5)
  - **Blocks**: Task 11 (needs Gradle)
  - **Blocked By**: Task 3 (CMakeLists.txt must exist)

  **References**:
  - Android Studio template: `tools/templates/gradle-projects/` - Standard Gradle structure

  **Acceptance Criteria**:
  - [ ] `build.gradle` files exist
  - [ ] `./gradlew tasks` succeeds
  - [ ] API 21 minimum configured

  **QA Scenarios**:
  ```
  Scenario: Gradle configuration valid
    Tool: Bash (gradlew)
    Steps:
      1. cd examples/android
      2. ./gradlew tasks --quiet
    Expected Result: Gradle tasks listed without errors
    Evidence: .sisyphus/evidence/task-04-gradle-config.txt
  ```

  **Commit**: YES
  - Message: `feat(android): add Gradle build configuration`
  - Files: `examples/android/build.gradle`, `examples/android/app/build.gradle`

- [ ] 5. **AndroidManifest.xml**

  **What to do**:
  - Create `examples/android/app/src/main/AndroidManifest.xml`
  - Declare NativeActivity as main activity
  - Set `android:screenOrientation="portrait"` (lock orientation)
  - Set `android:minSdkVersion="21"`
  - Add required permissions (none needed for basic demo)

  **Must NOT do**:
  - Add unnecessary permissions
  - Enable screen rotation

  **Recommended Agent Profile**:
  - **Category**: `quick`
    - Reason: Simple manifest file
  - **Skills**: []

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 1 (with Tasks 1-4)
  - **Blocks**: Task 6 (needs manifest)
  - **Blocked By**: None

  **References**:
  - NDK sample: `native-activity/AndroidManifest.xml` - Standard NativeActivity manifest

  **Acceptance Criteria**:
  - [ ] Manifest declares NativeActivity
  - [ ] Portrait orientation locked
  - [ ] API 21 minimum

  **QA Scenarios**:
  ```
  Scenario: Manifest validation
    Tool: Bash (aapt)
    Steps:
      1. aapt dump badging app/build/outputs/apk/debug/app-debug.apk | grep -i orientation
    Expected Result: "portrait" orientation in output
    Evidence: .sisyphus/evidence/task-05-manifest-check.txt
  ```

  **Commit**: YES
  - Message: `feat(android): add AndroidManifest.xml`
  - Files: `examples/android/app/src/main/AndroidManifest.xml`

- [ ] 6. **NativeActivity Entry Point**

  **What to do**:
  - Create `examples/android/app/src/main/cpp/main.cpp`
  - Implement `android_main(struct android_app* app)` entry point
  - Initialize `android_app` structure with `android_native_app_glue`
  - Set up event loop skeleton (empty handlers for now)
  - Add basic logging with `__android_log_print()`

  **Must NOT do**:
  - Call vg_lite functions yet (just skeleton)
  - Implement EGL yet

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high`
    - Reason: NativeActivity lifecycle requires careful setup
  - **Skills**: []

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 2 (with Tasks 7-9)
  - **Blocks**: Task 7 (needs app loop)
  - **Blocked By**: Task 3 (CMakeLists), Task 4 (Gradle), Task 5 (Manifest)

  **References**:
  - NDK sample: `sources/android/native-activity/main.cpp` - Standard entry point pattern

  **Acceptance Criteria**:
  - [ ] `android_main()` function exists
  - [ ] Build succeeds
  - [ ] APK installs and launches

  **QA Scenarios**:
  ```
  Scenario: APK build with skeleton
    Tool: Bash (gradlew)
    Steps:
      1. cd examples/android && ./gradlew assembleDebug
      2. ls -la app/build/outputs/apk/debug/app-debug.apk
    Expected Result: APK file exists (size > 0)
    Evidence: .sisyphus/evidence/task-06-apk-exists.txt
  ```

  **Commit**: YES
  - Message: `feat(android): add NativeActivity entry point skeleton`
  - Files: `examples/android/app/src/main/cpp/main.cpp`

- [ ] 7. **EGL Initialization and Termination**

  **What to do**:
  - Implement `init_egl(android_app* app)` function
  - Get EGL display with `eglGetDisplay()`
  - Initialize EGL with `eglInitialize()`
  - Choose config with `eglChooseConfig()` (RGBA8888, ES3)
  - Create surface with `eglCreateWindowSurface()`
  - Create context with `eglCreateContext()` (ES 3.0)
  - Make current with `eglMakeCurrent()`
  - Implement `term_egl()` for cleanup
  - Call from `APP_CMD_INIT_WINDOW` and `APP_CMD_TERM_WINDOW` handlers

  **Must NOT do**:
  - Call vg_lite functions yet
  - Create multiple contexts

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high`
    - Reason: EGL lifecycle is critical and error-prone
  - **Skills**: []

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 2 (with Tasks 6, 8-9)
  - **Blocks**: Task 8 (needs EGL context)
  - **Blocked By**: Task 6 (needs app loop)

  **References**:
  - NDK sample: `sources/android/native-activity/main.cpp` - EGL init pattern

  **Acceptance Criteria**:
  - [ ] `init_egl()` creates valid EGL context
  - [ ] `term_egl()` releases all resources
  - [ ] Logcat shows "EGL init success"

  **QA Scenarios**:
  ```
  Scenario: EGL initialization
    Tool: Bash (adb + logcat)
    Preconditions: APK built and installed
    Steps:
      1. adb shell am start -n com.example.vglitedemo/.android.app.NativeActivity
      2. adb logcat -d -s EGL | grep -i "init\|error"
    Expected Result: "EGL init success" present
    Evidence: .sisyphus/evidence/task-07-egl-init.log
  ```

  **Commit**: YES
  - Message: `feat(android): implement EGL init/term lifecycle`
  - Files: `examples/android/app/src/main/cpp/main.cpp`

- [ ] 8. **vg_lite Android Integration**

  **What to do**:
  - Call `vg_lite_init(width, height)` after EGL is ready
  - Get EGL handles: `eglGetCurrentDisplay()`, `eglGetCurrentContext()`, `eglGetCurrentSurface(EGL_DRAW)`
  - Call `vg_lite_set_gl_context(display, surface, context, 0)` with fbo_id=0
  - Handle errors with `__android_log_print()`
  - Call `vg_lite_close()` in termination handler

  **Must NOT do**:
  - Call vg_lite_clear() yet
  - Create vg_lite_buffer (use dummy for GL backend)

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high`
    - Reason: vg_lite context setup is critical
  - **Skills**: []

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 2 (with Tasks 6-7, 9)
  - **Blocks**: Task 9 (needs vg_lite init)
  - **Blocked By**: Task 7 (needs EGL context)

  **References**:
  - `vg_lite_tvg.cpp:vg_lite_set_gl_context()` - GL context setup
  - `examples/clear_demo/clear_demo.cpp` - Windows GL backend usage

  **Acceptance Criteria**:
  - [ ] `vg_lite_init()` returns `VG_LITE_SUCCESS`
  - [ ] `vg_lite_set_gl_context()` returns `VG_LITE_SUCCESS`
  - [ ] Logcat shows "VGLITE init success"

  **QA Scenarios**:
  ```
  Scenario: vg_lite initialization
    Tool: Bash (adb + logcat)
    Preconditions: EGL initialized
    Steps:
      1. adb logcat -d -s VGLITE ThorVG | grep -i "init\|success"
    Expected Result: "VGLITE init success" present
    Evidence: .sisyphus/evidence/task-08-vglite-init.log
  ```

  **Commit**: YES
  - Message: `feat(android): integrate vg_lite with Android EGL`
  - Files: `examples/android/app/src/main/cpp/main.cpp`

- [ ] 9. **Clear Demo Render Loop**

  **What to do**:
  - Create dummy `vg_lite_buffer_t` with window dimensions (memory unused for GL)
  - Implement color cycling: 5 colors (Red, Green, Blue, Yellow, Cyan)
  - Call `vg_lite_clear(&buffer, nullptr, color)` with current color
  - Call `vg_lite_finish()` to execute rendering
  - Call `eglSwapBuffers()` to display
  - Sleep for 1000ms between colors

  **Must NOT do**:
  - Allocate actual vg_lite_buffer memory (GL backend doesn't need it)
  - Use more than 5 colors

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high`
    - Reason: Render loop requires coordination of EGL and vg_lite
  - **Skills**: []

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 2 (with Tasks 6-8)
  - **Blocks**: Tasks 10-12 (needs demo to test)
  - **Blocked By**: Task 8 (needs vg_lite init)

  **References**:
  - `examples/clear_demo/clear_demo.cpp:main()` - Windows render loop pattern

  **Acceptance Criteria**:
  - [ ] Screen shows color cycling
  - [ ] All 5 colors visible
  - [ ] Cycle interval ~1000ms

  **QA Scenarios**:
  ```
  Scenario: Color cycling visible
    Tool: Bash (adb screencap)
    Preconditions: App running
    Steps:
      1. adb shell screencap -p /sdcard/cap1.png && adb pull /sdcard/cap1.png
      2. sleep 1.0
      3. adb shell screencap -p /sdcard/cap2.png && adb pull /sdcard/cap2.png
    Expected Result: cap1.png and cap2.png have different colors
    Evidence: .sisyphus/evidence/task-09-screencap/
  ```

  **Commit**: YES
  - Message: `feat(android): implement color cycling demo`
  - Files: `examples/android/app/src/main/cpp/main.cpp`

- [ ] 10. **Windows Regression Test**

  **What to do**:
  - Build Windows SW backend with `VGLITE_USE_GL_BACKEND=OFF`
  - Build Windows GL backend with `VGLITE_USE_GL_BACKEND=ON`
  - Run both demos and verify color cycling works

  **Must NOT do**:
  - Modify any Windows code
  - Skip this verification

  **Recommended Agent Profile**:
  - **Category**: `quick`
    - Reason: Build and run existing code
  - **Skills**: []

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 3 (with Tasks 11-12)
  - **Blocks**: Task 12 (needs confirmation)
  - **Blocked By**: Task 9 (demo must be complete)

  **References**:
  - `examples/clear_demo/clear_demo.cpp` - Windows demo

  **Acceptance Criteria**:
  - [ ] Windows SW build succeeds
  - [ ] Windows GL build succeeds
  - [ ] Both demos show color cycling

  **QA Scenarios**:
  ```
  Scenario: Windows SW backend unchanged
    Tool: Bash (cmake)
    Steps:
      1. cd examples && cmake -B build_sw -DVGLITE_USE_GL_BACKEND=OFF
      2. cmake --build build_sw --config Release
      3. ls -la build_sw/Release/clear_demo.exe
    Expected Result: clear_demo.exe exists
    Evidence: .sisyphus/evidence/task-10-win-sw.log
  ```

  **Commit**: NO

- [ ] 11. **Android Build Verification**

  **What to do**:
  - Clean build: `./gradlew clean`
  - Full build: `./gradlew assembleDebug`
  - Verify APK exists and has reasonable size

  **Recommended Agent Profile**:
  - **Category**: `quick`
    - Reason: Standard build verification
  - **Skills**: []

  **Parallelization**:
  - **Can Run In Parallel**: YES
  - **Parallel Group**: Wave 3 (with Tasks 10, 12)
  - **Blocks**: Task 12 (needs APK)
  - **Blocked By**: Task 9

  **Acceptance Criteria**:
  - [ ] `./gradlew assembleDebug` succeeds
  - [ ] APK size > 100KB

  **QA Scenarios**:
  ```
  Scenario: Android build success
    Tool: Bash (gradlew)
    Steps:
      1. cd examples/android && ./gradlew clean assembleDebug
      2. ls -la app/build/outputs/apk/debug/app-debug.apk
    Expected Result: APK exists
    Evidence: .sisyphus/evidence/task-11-android-build.txt
  ```

  **Commit**: NO

- [ ] 12. **Android Runtime Test**

  **What to do**:
  - Install APK: `adb install -r app-debug.apk`
  - Launch app
  - Capture logcat for 10 seconds
  - Verify init success messages
  - Capture screenshots showing color cycling

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high`
    - Reason: Requires device/emulator interaction
  - **Skills**: []

  **Parallelization**:
  - **Can Run In Parallel**: NO
  - **Parallel Group**: Wave 3 (depends on Tasks 10-11)
  - **Blocks**: Final verification
  - **Blocked By**: Tasks 9, 10, 11

  **Acceptance Criteria**:
  - [ ] APK installs without error
  - [ ] App launches and runs
  - [ ] Screenshots show color cycling

  **QA Scenarios**:
  ```
  Scenario: Full Android runtime test
    Tool: Bash (adb)
    Steps:
      1. adb install -r app/build/outputs/apk/debug/app-debug.apk
      2. adb logcat -c && adb shell am start -n com.example.vglitedemo/.android.app.NativeActivity
      3. sleep 5 && adb logcat -d > logcat.txt
    Expected Result: All steps succeed
    Evidence: .sisyphus/evidence/task-12-runtime/
  ```

  **Commit**: YES
  - Message: `test(android): verify runtime color cycling`
  - Files: None (evidence only)

---

## Final Verification Wave (MANDATORY — after ALL implementation tasks)

> 4 review agents run in PARALLEL. ALL must APPROVE. Present consolidated results to user and get explicit "okay" before completing.

- [ ] F1. **Plan Compliance Audit** — `oracle`
  Read the plan end-to-end. For each "Must Have": verify implementation exists. For each "Must NOT Have": search for forbidden patterns. Check evidence files exist in .sisyphus/evidence/.
  Output: `Must Have [N/N] | Must NOT Have [N/N] | Tasks [N/N] | VERDICT: APPROVE/REJECT`

- [ ] F2. **Code Quality Review** — `unspecified-high`
  Run `tsc --noEmit` + linter (Windows), `./gradlew assembleDebug` (Android). Review changed files for platform isolation, macro guards.
  Output: `Windows Build [PASS/FAIL] | Android Build [PASS/FAIL] | Platform Isolation [CLEAN/N issues] | VERDICT`

- [ ] F3. **Real Manual QA** — `unspecified-high`
  Install APK on device/emulator, run demo, capture screenshots of all 5 colors, verify Windows demo still works.
  Output: `Windows Demo [PASS/FAIL] | Android Demo [PASS/FAIL] | Colors [N/N] | VERDICT`

- [ ] F4. **Scope Fidelity Check** — `deep`
  For each task: verify only specified changes were made, no scope creep, no Windows breakage.
  Output: `Tasks [N/N compliant] | Windows Breakage [NONE/N issues] | Unaccounted [CLEAN/N files] | VERDICT`

---

## Commit Strategy

| Commit | Scope | Message | Pre-commit |
|--------|-------|---------|------------|
| 1 | Platform macros | `feat(android): add platform detection macros` | Windows build |
| 2 | Memory allocation | `feat(android): add aligned memory allocation for Android` | Windows build |
| 3 | CMake scaffolding | `feat(android): add CMakeLists.txt for Android build` | cmake configure |
| 4 | Gradle config | `feat(android): add Gradle build configuration` | gradlew clean |
| 5 | NativeActivity | `feat(android): add NativeActivity entry point` | gradlew assembleDebug |
| 6 | EGL lifecycle | `feat(android): implement EGL init/term lifecycle` | gradlew assembleDebug |
| 7 | vg_lite integration | `feat(android): integrate vg_lite with Android EGL` | gradlew assembleDebug |
| 8 | Clear demo | `feat(android): implement color cycling demo` | gradlew installDebug + logcat |

---

## Success Criteria

### Verification Commands
```bash
# Windows regression test
cd examples && cmake -B build_sw -DVGLITE_USE_GL_BACKEND=OFF && cmake --build build_sw --config Release
./build_sw/Release/clear_demo.exe  # Should show centered 256x256 color cycling

cd examples && cmake -B build_gl -DVGLITE_USE_GL_BACKEND=ON && cmake --build build_gl --config Release
./build_gl/Release/clear_demo.exe  # Should show fullscreen color cycling

# Android build test
cd examples/android && ./gradlew assembleDebug
ls -la app/build/outputs/apk/debug/app-debug.apk  # Should exist

# Android runtime test
adb install -r app/build/outputs/apk/debug/app-debug.apk
adb shell am start -n com.example.vglitedemo/.android.app.NativeActivity
adb logcat -s VGLITE EGL ThorVG  # Should show init success
adb shell screencap -p /sdcard/test.png && adb pull /sdcard/test.png  # Should show solid color
```

### Final Checklist
- [ ] All "Must Have" present (platform macros, EGL handling, CMake, Gradle, APK)
- [ ] All "Must NOT Have" absent (ThorVG mods, Windows breakage, SW backend on Android)
- [ ] Windows SW and GL backends still work
- [ ] Android APK builds, installs, runs
- [ ] 5-color cycling visible on Android

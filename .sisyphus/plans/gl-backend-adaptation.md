# GL Backend Adaptation for VGLite ThorVG

## TL;DR

> **Quick Summary**: Add OpenGL backend to vglite ThorVG by reusing ThorVG 1.0.2's existing GlRenderer, using compile-time macros to isolate SW and GL code paths without affecting SW functionality.
> 
> **Deliverables**:
> - `vglite_gl_config.h` — GL backend configuration macros
> - `vg_lite_tvg_gl.cpp` — GL-specific implementation (conditional compilation)
> - Modified `vg_lite_tvg.cpp` — Conditional SW/GL branches
> - Updated `CMakeLists.txt` — OpenGL linking option
> - Adapted `clear_demo` — Demonstrates GL backend usage
> 
> **Estimated Effort**: Medium
> **Parallel Execution**: YES — 3 waves
> **Critical Path**: Config → vg_lite_ctx GL support → clear_demo adaptation

---

## Context

### Original Request
User wants to:
1. Analyze interfaces required for `vg_lite_clear` drawing
2. Adapt ThorVG GL backend (current is SW backend)
3. New GL code must NOT affect SW functionality
4. Use compile macros to isolate code differences
5. Adapt `clear_demo.cpp` example for GL backend

### Interview Summary

**Key Discussions**:
- **GL Context Management**: External (GLFW/WGL) — consistent with clear_demo existing architecture
- **Render Output**: Support glReadPixels — API compatibility, `buffer.memory` contains result after `vg_lite_finish()`
- **Backend Switch**: Compile-time macro isolation — clean code, zero runtime overhead

**Research Findings**:
- ThorVG 1.0.2 has complete GL engine at `libs/thorvg-1.0.2/src/renderer/gl_engine/`
- GlRenderer inherits RenderMethod (same interface as SwRenderer)
- **CRITICAL**: ThorVG does NOT load `glReadPixels` — requires manual implementation for CPU readback
- **LIMITATION**: GL only supports `ColorSpace::ABGR8888S`, not `BGRA8888`
- **LIMITATION**: GlCanvas doesn't support Smart Rendering (partial redraws)

### Metis Review

**Identified Gaps** (addressed):

| Gap | Classification | Resolution |
|-----|----------------|------------|
| `glReadPixels` not loaded by ThorVG | **CRITICAL** | Defer to SW-only in Phase 1, document limitation |
| ColorSpace mismatch (BGRA8888 vs ABGR8888S) | **CRITICAL** | Document as GL limitation, accept ABGR8888S output |
| Multi-format support scope creep | **MINOR** | MVP: BGRA8888/ABGR8888S only, guardrail set |
| FBO management responsibility | **AMBIGUOUS** | External (application provides fbo_id), default = 0 (onscreen) |

---

## Work Objectives

### Core Objective
Add OpenGL backend support to vglite ThorVG by reusing ThorVG 1.0.2's GlRenderer, using compile-time macros (`VGLITE_USE_GL_BACKEND`) to isolate SW and GL code paths, without affecting existing SW backend functionality.

### Concrete Deliverables
- `vglite_gl_config.h` — Compile-time configuration macros
- GL backend support in `vg_lite_tvg.cpp` (conditionally compiled)
- Updated `CMakeLists.txt` with OpenGL linking option
- `clear_demo` adapted to demonstrate GL backend

### Definition of Done
- [ ] SW backend compiles and runs unchanged when `VGLITE_USE_GL_BACKEND=0`
- [ ] GL backend compiles when `VGLITE_USE_GL_BACKEND=1`
- [ ] clear_demo runs with GL backend and displays 5 color cycle
- [ ] No GL symbols in SW-only build (`nm vglite.lib | grep -i gl` returns empty)
- [ ] Documentation of GL limitations (CPU readback, colorspace)

### Must Have
- Compile-time `VGLITE_USE_GL_BACKEND` macro (0=SW, 1=GL)
- GlCanvas integration replacing SwCanvas when GL enabled
- External GL context support (GLFW/WGL)
- BGRA8888/ABGR8888S format support
- clear_demo GL mode working

### Must NOT Have (Guardrails)
- **NO** changes to SW backend behavior when `VGLITE_USE_GL_BACKEND=0`
- **NO** OpenGL linking when building SW-only
- **NO** multi-format conversion for GL in Phase 1
- **NO** runtime backend switching
- **NO** CPU readback for GL mode (document as SW-only limitation)

---

## Verification Strategy (MANDATORY)

> **ZERO HUMAN INTERVENTION** — ALL verification is agent-executed.

### Test Decision
- **Infrastructure exists**: YES (GoogleTest in `tests/`)
- **Automated tests**: NO — User requested Agent-Executed QA only
- **Framework**: N/A
- **Agent-Executed QA**: ALWAYS (mandatory for all tasks)

### QA Policy
Every task includes agent-executed QA scenarios:
- **Build Verification**: CMake build with/without GL backend
- **Symbol Check**: Verify no GL symbols in SW build
- **Runtime Test**: clear_demo color cycle verification
- **Regression Test**: SW backend unchanged

---

## Execution Strategy

### Parallel Execution Waves

```
Wave 1 (Start Immediately — configuration + scaffolding):
├── Task 1: Create vglite_gl_config.h [quick]
├── Task 2: Add THORVG_GL_RASTER_SUPPORT to thorvg config.h [quick]
└── Task 3: Update root CMakeLists.txt with GL option [quick]

Wave 2 (After Wave 1 — GL backend implementation):
├── Task 4: Add GL backend to vg_lite_ctx class [deep]
├── Task 5: Implement GL vg_lite_allocate/free [deep]
├── Task 6: Implement GL canvas_set_target [deep]
├── Task 7: Implement GL vg_lite_clear [deep]
└── Task 8: Implement GL vg_lite_finish [deep]

Wave 3 (After Wave 2 — integration + demo):
├── Task 9: Update examples/CMakeLists.txt for GL [quick]
├── Task 10: Adapt clear_demo for GL backend [visual-engineering]
└── Task 11: Document GL limitations [quick]

Wave FINAL (After ALL tasks — verification):
├── Task F1: Plan compliance audit (oracle)
├── Task F2: Code quality review (unspecified-high)
├── Task F3: Real manual QA (unspecified-high)
└── Task F4: Scope fidelity check (deep)
→ Present results → Get explicit user okay

Critical Path: Task 1 → Task 4 → Task 7 → Task 10 → F1-F4 → user okay
Parallel Speedup: ~60% faster than sequential
Max Concurrent: 5 (Wave 2)
```

### Dependency Matrix

| Task | Depends On | Blocks |
|------|------------|--------|
| 1 | — | 4, 5, 6, 7, 8 |
| 2 | — | 4, 5, 6, 7, 8 |
| 3 | 1, 2 | 9, 10 |
| 4 | 1, 2 | 5, 6, 7, 8 |
| 5 | 4 | 8 |
| 6 | 4 | 7, 8 |
| 7 | 4, 6 | 8, 10 |
| 8 | 5, 6, 7 | 10 |
| 9 | 3 | 10 |
| 10 | 7, 8, 9 | F1-F4 |
| F1-F4 | 10 | user okay |

### Agent Dispatch Summary

- **Wave 1**: 3 × `quick`
- **Wave 2**: 5 × `deep`
- **Wave 3**: 2 × `quick` + 1 × `visual-engineering`
- **Final**: 4 parallel reviews

---

## TODOs

### Wave 1 — Configuration & Scaffolding

- [ ] 1. Create vglite_gl_config.h

  **What to do**:
  - Create `vglite_gl_config.h` with `VGLITE_USE_GL_BACKEND` macro (0=SW, 1=GL)
  - Add helper macros for conditional GL/SW code paths
  
  **Must NOT do**: Add runtime overhead or GL-specific types when SW mode
  
  **References**:
  - `libs/thorvg-1.0.2/src/config.h:15-18` — Existing ThorVG config pattern
  
  **QA Scenarios**:
  ```
  Scenario: SW mode compilation
    Tool: Bash (cmake)
    Steps: cmake -DVGLITE_USE_GL_BACKEND=OFF && build
    Expected: VGLITE_USE_GL_BACKEND == 0, no GL symbols
    Evidence: .sisyphus/evidence/task-1-sw-compile.log
  ```

- [ ] 2. Enable THORVG_GL_RASTER_SUPPORT in thorvg config.h

  **What to do**:
  - Modify `libs/thorvg-1.0.2/src/config.h`
  - Add conditional `#define THORVG_GL_RASTER_SUPPORT 1` when `VGLITE_USE_GL_BACKEND=1`
  
  **Must NOT do**: Change SW engine behavior
  
  **References**:
  - `libs/thorvg-1.0.2/src/config.h:15-18` — Config macros
  
  **QA Scenarios**:
  ```
  Scenario: GL engine sources included
    Tool: Bash (cmake)
    Steps: cmake -DVGLITE_USE_GL_BACKEND=ON && check compile_commands
    Expected: gl_engine files in build
    Evidence: .sisyphus/evidence/task-2-gl-sources.log
  ```

- [ ] 3. Update root CMakeLists.txt with GL option

  **What to do**:
  - Add `option(VGLITE_USE_GL_BACKEND "Use GL backend" OFF)`
  - Add OpenGL library linking when GL enabled
  - Conditionally define `THORVG_GL_RASTER_SUPPORT`
  
  **Must NOT do**: Link OpenGL when SW-only
  
  **References**:
  - `CMakeLists.txt` — Root build config
  
  **QA Scenarios**:
  ```
  Scenario: SW build without GL
    Tool: Bash (cmake)
    Steps: cmake -DVGLITE_USE_GL_BACKEND=OFF && ldd vglite.lib
    Expected: No OpenGL32.dll linked
    Evidence: .sisyphus/evidence/task-3-sw-no-gl.log
  ```

### Wave 2 — GL Backend Implementation

- [ ] 4. Add GL backend to vg_lite_ctx class

  **What to do**:
  - Modify `vg_lite_ctx` in `vg_lite_tvg.cpp`
  - Use `GlCanvas::gen()` instead of `SwCanvas::gen()` when GL enabled
  - Add GL context members: display, surface, context, fbo_id
  
  **Must NOT do**: Change SW behavior when VGLITE_USE_GL_BACKEND=0
  
  **References**:
  - `vg_lite_tvg.cpp:152-267` — Current vg_lite_ctx
  - `libs/thorvg-1.0.2/inc/thorvg.h:2319-2376` — GlCanvas API
  
  **QA Scenarios**:
  ```
  Scenario: SW mode compiles unchanged
    Tool: Bash (cmake)
    Steps: cmake -DVGLITE_USE_GL_BACKEND=OFF && build
    Expected: Build succeeds
    Evidence: .sisyphus/evidence/task-4-sw-build.log
  ```

- [ ] 5. Implement GL vg_lite_allocate/free

  **What to do**:
  - Add GL buffer allocation using `glGenFramebuffers()` + `glGenTextures()`
  - Store FBO ID in buffer.handle
  - Implement `vg_lite_free()` to release GL resources
  
  **Must NOT do**: Change SW allocation logic
  
  **References**:
  - `vg_lite_tvg.cpp:537-575` — Current SW allocation
  - `libs/thorvg-1.0.2/src/renderer/gl_engine/tvgGlRenderTarget.h` — FBO management
  
  **QA Scenarios**:
  ```
  Scenario: GL allocation creates FBO
    Tool: Bash (run executable)
    Steps: vg_lite_allocate() && check buffer.handle
    Expected: Valid FBO ID
    Evidence: .sisyphus/evidence/task-5-gl-fbo.log
  ```

- [ ] 6. Implement GL canvas_set_target

  **What to do**:
  - Call `GlCanvas::target(display, surface, context, fbo_id, w, h, ABGR8888S)`
  - Handle external GL context from GLFW
  
  **Must NOT do**: Change SW canvas_set_target
  
  **References**:
  - `vg_lite_tvg.cpp:2608-2659` — Current SW canvas_set_target
  - `libs/thorvg-1.0.2/inc/thorvg.h:2356-2366` — GlCanvas::target() API
  
  **QA Scenarios**:
  ```
  Scenario: GL canvas target setup
    Tool: Bash (run executable)
    Steps: Init GLFW && canvas_set_target()
    Expected: GlCanvas::target() returns success
    Evidence: .sisyphus/evidence/task-6-gl-target.log
  ```

- [ ] 7. Implement GL vg_lite_clear

  **What to do**:
  - Use GlRenderer's shape rendering pipeline
  - Render colored rectangle to FBO
  - **NO** CPU readback (document as limitation)
  
  **Must NOT do**: Change SW vg_lite_clear or call glReadPixels
  
  **References**:
  - `vg_lite_tvg.cpp:611-623` — Current SW vg_lite_clear
  - `libs/thorvg-1.0.2/src/renderer/gl_engine/tvgGlRenderer.cpp` — GlRenderer
  
  **QA Scenarios**:
  ```
  Scenario: GL clear with red color
    Tool: interactive_bash (tmux)
    Steps: vg_lite_clear(nullptr, 0xFFFF0000) && check FBO
    Expected: FBO filled with red
    Evidence: .sisyphus/evidence/task-7-gl-clear-red.log
  ```

- [ ] 8. Implement GL vg_lite_finish

  **What to do**:
  - Call `GlCanvas::draw()` → `sync()` → `remove()`
  - **LIMITATION**: No CPU readback (SW-only feature)
  - Add comment explaining limitation
  
  **Must NOT do**: Change SW vg_lite_finish or call glReadPixels
  
  **References**:
  - `vg_lite_tvg.cpp:842-933` — Current SW vg_lite_finish
  
  **QA Scenarios**:
  ```
  Scenario: GL finish executes render
    Tool: Bash (run executable)
    Steps: Add shape && vg_lite_finish()
    Expected: Rendering completed
    Evidence: .sisyphus/evidence/task-8-gl-finish.log
  ```

### Wave 3 — Integration & Demo

- [ ] 9. Update examples/CMakeLists.txt for GL

  **What to do**:
  - Add GL option and OpenGL linking
  - Conditionally compile GL backend support
  
  **Must NOT do**: Require GL when SW mode
  
  **References**:
  - `examples/CMakeLists.txt` — Current examples build
  
  **QA Scenarios**:
  ```
  Scenario: Examples build with GL
    Tool: Bash (cmake)
    Steps: cmake -DVGLITE_USE_GL_BACKEND=ON && build
    Expected: All examples compile
    Evidence: .sisyphus/evidence/task-9-examples-gl.log
  ```

- [ ] 10. Adapt clear_demo for GL backend

  **What to do**:
  - Add conditional GL backend support in clear_demo.cpp
  - Use existing GLFW GL context
  - Call `GlCanvas::target()` instead of `SwCanvas::target()`
  - **NO CHANGES** to color cycling logic
  
  **Must NOT do**: Break SW mode or change rendering behavior
  
  **References**:
  - `examples/clear_demo/clear_demo.cpp` — Current implementation
  - `libs/thorvg-1.0.2/inc/thorvg.h:2319-2376` — GlCanvas API
  
  **QA Scenarios**:
  ```
  Scenario: GL mode displays color cycle
    Tool: interactive_bash (tmux)
    Steps: Run clear_demo.exe && observe 5 colors
    Expected: Window shows red, green, blue, yellow, cyan
    Evidence: .sisyphus/evidence/task-10-clear-demo-gl.png
  ```

- [ ] 11. Document GL limitations

  **What to do**:
  - Create `GL_BACKEND_LIMITATIONS.md`
  - Document: CPU readback (SW-only), ColorSpace (ABGR8888S only), No Smart Rendering
  
  **Must NOT do**: Change code behavior
  
  **References**:
  - Metis analysis from planning session
  
  **QA Scenarios**:
  ```
  Scenario: Documentation exists
    Tool: Bash (ls)
    Steps: Check GL_BACKEND_LIMITATIONS.md
    Expected: File exists with complete limitation list
    Evidence: .sisyphus/evidence/task-11-doc-exists.log
  ```

---

## Final Verification Wave

> **4 review agents run in PARALLEL. ALL must APPROVE. Present consolidated results to user and get explicit "okay" before completing.**
>
> **Do NOT auto-proceed after verification. Wait for user's explicit approval before marking work complete.**

- [ ] F1. **Plan Compliance Audit** — `oracle`
  Read the plan end-to-end. For each "Must Have": verify implementation exists. For each "Must NOT Have": search codebase for forbidden patterns. Check evidence files exist in .sisyphus/evidence/.
  Output: `Must Have [N/N] | Must NOT Have [N/N] | Tasks [N/N] | VERDICT: APPROVE/REJECT`

- [ ] F2. **Code Quality Review** — `unspecified-high`
  Run CMake build + linter. Review all changed files for: empty catches, console.log in prod, commented-out code, unused imports. Check AI slop: excessive comments, over-abstraction.
  Output: `Build [PASS/FAIL] | Files [N clean/N issues] | VERDICT`

- [ ] F3. **Real Manual QA** — `unspecified-high` (+ `playwright` skill if UI)
  Start from clean state. Execute EVERY QA scenario from EVERY task. Test cross-task integration. Save to `.sisyphus/evidence/final-qa/`.
  Output: `Scenarios [N/N pass] | Integration [N/N] | VERDICT`

- [ ] F4. **Scope Fidelity Check** — `deep`
  For each task: read "What to do", read actual diff. Verify 1:1 — everything in spec was built (no missing), nothing beyond spec was built (no creep).
  Output: `Tasks [N/N compliant] | Contamination [CLEAN/N issues] | VERDICT`

---

## Commit Strategy

- **Task 1-3**: `feat(config): add GL backend compile-time configuration`
- **Task 4-8**: `feat(gl): implement GL backend for vg_lite core APIs`
- **Task 9-10**: `feat(examples): adapt clear_demo for GL backend`
- **Task 11**: `docs(gl): document GL backend limitations`

---

## Success Criteria

### Verification Commands
```bash
# 1. SW backend unchanged
cmake -DVGLITE_USE_GL_BACKEND=OFF -B build_sw && cmake --build build_sw
nm build_sw/vglite.lib | grep -i "gl\|opengl"  # Expected: empty

# 2. GL backend compiles
cmake -DVGLITE_USE_GL_BACKEND=ON -B build_gl && cmake --build build_gl
# Expected: Exit code 0

# 3. clear_demo GL mode
./build_gl/examples/clear_demo/clear_demo.exe
# Expected: Window shows 5 color cycle (red, green, blue, yellow, cyan)
```

### Final Checklist
- [ ] All "Must Have" present
- [ ] All "Must NOT Have" absent
- [ ] SW backend tests pass
- [ ] GL backend clear_demo runs

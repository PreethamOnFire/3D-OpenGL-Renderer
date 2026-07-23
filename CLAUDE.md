# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System

This is a **Visual Studio 2022** project (MSVC v143 toolset, C++17, Windows 10 SDK). There is no CMake or Makefile.

- Open `GLFW.sln` in Visual Studio and build with **Debug|x64** or **Release|x64**.
- Via CLI: `msbuild GLFW.sln /p:Configuration=Debug /p:Platform=x64`
- The working directory when running must be `GLFW/` so that relative shader paths (`src/Shaders/...`) and asset paths (`assets/...`) resolve correctly.

## Dependencies

All dependencies live under `Dependencies/` and are pre-built for x64:
- **GLFW** — window/input (`Dependencies/GLFW/`)
- **GLEW 2.1.0** (static, `GLEW_STATIC`) — OpenGL extension loading (`Dependencies/glew-2.1.0/`)
- **GLM 1.0.1** — math (`Dependencies/glm-1.0.1/`)
- **Assimp** — model loading (`Dependencies/Assimp/`)
- **stb_image** — texture loading (header-only, `GLFW/src/stb/stb_image.h`)

Linked libraries: `glfw3.lib`, `opengl32.lib`, `glew32s.lib`, `assimp-vc143-mt.lib`, plus standard Windows libs.

## Architecture

### Entry Point and Main Loop

`GLFW/src/Core/Application.h/.cpp` is the base class: it owns `Window`, `Renderer`, `InputManager`, and `Clock`, and defines `run()` as the final (non-overridable) main loop. Subclasses override `onInit()`, `onUpdate()`, `onRender()`, `onImGui()`, and `onShutdown()`.

`GLFW/src/RendererApp.h/.cpp` is the concrete subclass — it owns the `Scene`, a `ShaderPipelineLibrary`, and a `MaterialLibrary`, wires up shaders/models/lights in `onInit()`, and `main()` lives at the bottom of this file.

### Main Loop and Rendering Pipeline

```
Application::run()
  → input->captureCursor(), onInit()
  → loop:
      → window->pollEvents(), clock.tick()
      → input->update(window)
      → renderer->getCamera().update(*input, deltaTime)   // WASD + QE + mouse look
      → onUpdate()                                        // per-frame game logic (e.g. Normandy bobbing)
      → renderer->clear()
      → onRender()
          → scene->render(*renderer, materials, pipeline)
              → skybox->render(*cam)
              → updateLightUniforms(pipeline, viewPos)     // once per frame
              → renderer->bindGlobalUniforms(pipeline)     // time, deltaTime — once per frame
              → for each model:
                  → model->render(renderer, materials)
                      → sceneNode tree traversal
                          → for each mesh: material.bind() → renderer.drawTriangles(mesh, pipeline) → material.unbind()
      → onImGui(), window->swapBuffers()
```

`Renderer::drawTriangles(Mesh&, ShaderPipeline&)` computes MVP, modelMatrix, and normalMatrix and uploads them to the given pipeline before calling `glDrawElements`. It does not bind the pipeline or apply GL state itself — that's `Material::bind()`'s job (see below).

### Scene Graph

- **`Scene`** — top-level container holding a list of `Model`s, a list of `Light`s, and an optional `SkyBox`. Owns everything via `unique_ptr<Model>`.
- **`Model`** — has a name/id, a root `SceneNode`, and a transform. Can be loaded from file (`LOADED_MODEL`) or be a primitive (`PRIMITIVE_CUBE`, `PRIMITIVE_SPHERE`, `PRIMITIVE_PLANE`). Materials are not owned by `Model` — they live in the app-owned `MaterialLibrary`, referenced by name. Transform calls on `Model` delegate to the root `SceneNode`.
- **`SceneNode`** — tree node with local position/rotation/scale, a list of `unique_ptr<Mesh>`, and child `SceneNode`s. Computes `localTransform` and propagates `globalTransform` down the tree. For each mesh, looks up its material by name in the `MaterialLibrary` and calls `renderer.drawTriangles()` bracketed by `material.bind()`/`unbind()`.
- **`Mesh`** — owns the GPU buffers (`unique_ptr<VertexArray>`, `VertexBuffer`, `IndexBuffer`), a value `modelMatrix`, and a `materialName` string used to look itself up in the `MaterialLibrary` at render time.

### Shading: ShaderPipeline → Material → MaterialLibrary

- **`ShaderPipeline`** (`GLFW/src/Core/ShaderPipeline.h/.cpp`) replaces the old `Shader` class. In addition to compiling/linking GLSL and the `setBool/setInt/setFloat/setVec3/setMat4` uniform setters, it owns GL render state (`depthTest`, `depthWrite`, `blending`, `faceCulling`, `cullFace`, `blendSrc`, `blendDst`). `applyState()` issues the corresponding `glEnable`/`glDisable`/`glDepthMask`/`glBlendFunc`/`glCullFace` calls; `bind()` is `glUseProgram` + `applyState()`; `restoreState()` reasserts an engine-wide baseline. `use()` is a bare `glUseProgram`, for uniform-only call sites (`Renderer::bindGlobalUniforms`, `Scene::updateLightUniforms`) that shouldn't re-toggle GL state.
- **`ShaderPipelineLibrary`** (`GLFW/src/Core/ShaderPipelineLibrary.h/.cpp`) — name-keyed `unordered_map<string, unique_ptr<ShaderPipeline>>`, owned by `RendererApp`. `load(name, vertPath, fragPath)` compiles and stores; `get(name)` returns a non-owning pointer.
- **`Material`** (`GLFW/src/Rendering/Material.h/.cpp`) owns a non-owning `ShaderPipeline*` plus named property maps (`float`/`vec3`/`mat4`) and named textures (`setTexture(type, texture, slot)`, auto-assigning GL texture units 0/1/2 for `"diffuse"/"specular"/"normal"` to match the hardcoded `material.diffuse0`/`specular0`/`normal0` samplers in the GLSL). `bind()` calls `pipeline->bind()`, uploads all properties/textures, and is the **only** place a pipeline gets bound during a draw. `unbind()` unbinds textures and calls `pipeline->restoreState()`.
- **`MaterialLibrary`** (`GLFW/src/Rendering/MaterialLibrary.h/.cpp`) — name-keyed `unordered_map<string, unique_ptr<Material>>`, owned by `RendererApp`. `create(name, pipeline)` constructs and stores; `get(name)` is nullable; `getOrDefault(name)` falls back to the entry keyed `"default"` (which the app must seed in `onInit()`).
- Model loading populates the library by naming convention: primitives create `modelName + "_default"`; Assimp-loaded models create `modelName + "_mat_" + materialIndex` per `aiMaterial`, and each `Mesh`'s `materialName` is computed with the same convention so no separate index table is needed.

### Lighting

`Scene::updateLightUniforms()` uploads all lights to the active lighting pipeline as a `lights[16]` uniform array, once per frame (not once per model). The `Light` struct supports three types (DIRECTIONAL=0, POINT=1, SPOT=2) and is mirrored identically in the GLSL shaders. Up to 16 lights are supported.

### Shader Conventions

Vertex shaders receive: `aPos` (loc 0), `aNorm` (loc 1), `aTex` (loc 2), plus uniforms `MVP`, `modelMatrix`, `normalMatrix`.

Fragment shaders expect:
- `uniform Material material` — diffuse/specular/normal maps + fallback vec3 colors + `shininess`
- `uniform Light[16] lights` + `uniform int numLights`
- `uniform vec3 ambientLight`, `uniform vec3 viewPos`
- `uniform float time`, `uniform float deltaTime` (bound by `Renderer::bindGlobalUniforms`)

Shader files live in `GLFW/src/Shaders/` (both vertex `.vs` and fragment `.fs` files). Paths are passed relative to the working directory at `ShaderPipeline` construction time.

### Water Shader

`SimpleWaterVertexShader.vs`/`SimpleWaterFragmentShader.fs` exist under `src/Shaders/` implementing sine-wave vertex animation with Fresnel-based fragment coloring, but are not currently wired up to any pipeline or model — `Scene::addWaterPlane()` exists but isn't called from `RendererApp::onInit()`.

### Asset Layout (relative to working directory `GLFW/`)

```
assets/
  models/      # OBJ and GLTF model files
  skybox/      # Six cubemap face images: right/left/top/bottom/front/back .jpg
src/
  Shaders/     # .vs / .fs GLSL files
```

## Key Design Notes

- `Scene::addModel()` returns a raw `Model*` (non-owning). The `Scene` retains ownership via `unique_ptr`. Pointers become dangling after `removeModel()`.
- Scales with a negative Y component (e.g. `(0.001f, -0.001f, 0.001f)`) are used throughout `RendererApp::onInit()` for models whose coordinate system doesn't match OpenGL's.
- The `normalMatrix` is `transpose(inverse(modelMatrix))`, computed CPU-side in `Renderer::drawTriangles()` and uploaded per draw call.
- `ShaderPipeline`/`Material` objects are owned by `RendererApp`'s `ShaderPipelineLibrary`/`MaterialLibrary` (both by value, `unordered_map<string, unique_ptr<T>>` internally) — no manual `new`/`delete`. `Mesh`, `Model`, and `SceneNode` only ever hold non-owning references (a name string or raw pointer) into these libraries. Models loaded via Assimp are wrapped immediately into `SceneNode` trees by `ModelLoader`, which populates the `MaterialLibrary` as a side effect rather than returning materials directly.

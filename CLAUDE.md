# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System

This is a **Visual Studio 2022** project (MSVC v143 toolset, C++17, Windows 10 SDK). There is no CMake or Makefile.

- Open `GLFW.sln` in Visual Studio and build with **Debug|x64** or **Release|x64**.
- Via CLI: `msbuild GLFW.sln /p:Configuration=Debug /p:Platform=x64`
- The working directory when running must be `GLFW/` so that relative shader paths (`src/VertexShaders/...`) and asset paths (`assets/...`) resolve correctly.

## Dependencies

All dependencies live under `Dependencies/` and are pre-built for x64:
- **GLFW** — window/input (`Dependencies/GLFW/`)
- **GLEW 2.1.0** (static, `GLEW_STATIC`) — OpenGL extension loading (`Dependencies/glew-2.1.0/`)
- **GLM 1.0.1** — math (`Dependencies/glm-1.0.1/`)
- **Assimp** — model loading (`Dependencies/Assimp/`)
- **stb_image** — texture loading (header-only, `GLFW/src/stb/stb_image.h`)

Linked libraries: `glfw3.lib`, `opengl32.lib`, `glew32s.lib`, `assimp-vc143-mt.lib`, plus standard Windows libs.

## Architecture

### Entry Point and Game Loop

`GLFW/src/Applcation.cpp` contains the `Game` class which owns the top-level lifecycle: init → `gameSetUp()` → `mainLoop()` → `cleanup()`. This is where scenes, shaders, and models are constructed and wired together.

### Rendering Pipeline

```
Game::mainLoop()
  → renderer->GetInput()        // camera movement (WASD + QE + mouse look)
  → scene->render(*renderer)
      → skybox->render(*cam)
      → for each model:
          → scene->updateLightUniforms(*shader, viewPos)
          → renderer->bindGlobalUniforms(*shader)   // time, deltaTime
          → model->render(renderer)
              → sceneNode tree traversal
                  → mesh->draw via renderer->DrawTriangles()
```

`Renderer::DrawTriangles()` computes MVP, modelMatrix, and normalMatrix and uploads them to the shader before calling `glDrawElements`.

### Scene Graph

- **`Scene`** — top-level container holding a list of `Model`s, a list of `Light`s, and an optional `SkyBox`. Owns everything via `unique_ptr<Model>`.
- **`Model`** — has a name/id, a root `SceneNode`, a shader pointer, and a flat `vector<Material*>`. Can be loaded from file (`LOADED_MODEL`) or be a primitive (`PRIMITIVE_CUBE`, `PRIMITIVE_SPHERE`, `PRIMITIVE_PLANE`). Transform calls on `Model` delegate to the root `SceneNode`.
- **`SceneNode`** — tree node with local position/rotation/scale, a list of `Mesh*` pointers, and child `SceneNode`s. Computes `localTransform` and propagates `globalTransform` down the tree. Calls `renderer.DrawTriangles()` for each mesh.
- **`Mesh`** — owns the GPU buffers (`VertexArray`, `VertexBuffer`, `IndexBuffer`) and a `modelMatrix` pointer. Two constructors: raw float array + layout, or `vector<Vertex>`.

### Lighting

`Scene::updateLightUniforms()` uploads all lights to the shader as a `lights[16]` uniform array before each model draw. The `Light` struct supports three types (DIRECTIONAL=0, POINT=1, SPOT=2) and is mirrored identically in the GLSL shaders. Up to 16 lights are supported.

### Shader Conventions

Vertex shaders receive: `aPos` (loc 0), `aNorm` (loc 1), `aTex` (loc 2), plus uniforms `MVP`, `modelMatrix`, `normalMatrix`.

Fragment shaders expect:
- `uniform Material material` — diffuse/specular/normal maps + fallback vec3 colors + `shininess`
- `uniform Light[16] lights` + `uniform int numLights`
- `uniform vec3 ambientLight`, `uniform vec3 viewPos`
- `uniform float time`, `uniform float deltaTime` (bound by `Renderer::bindGlobalUniforms`)

Shader files live in `GLFW/src/VertexShaders/` and `GLFW/src/FragmentShaders/`. Paths are passed relative to the working directory at `Shader` construction time.

### Water Shader

`SimpleWaterVertexShader.vs` animates vertex positions using three superimposed sine waves driven by `time` and per-wave `waveSpeed`/`waveAmplitude`/`waveFrequency` uniforms. The fragment shader (`SimpleWaterFragmentShader.fs`) applies Fresnel-based color blending and foam at wave crests. Wave parameters are currently set once in `gameSetUp()` via `WaterShader->setFloat(...)`.

### Asset Layout (relative to working directory `GLFW/`)

```
assets/
  models/      # OBJ and GLTF model files
  skybox/      # Six cubemap face images: right/left/top/bottom/front/back .jpg
src/
  VertexShaders/
  FragmentShaders/
```

## Key Design Notes

- `Scene::addModel()` returns a raw `Model*` (non-owning). The `Scene` retains ownership via `unique_ptr`. Pointers become dangling after `removeModel()`.
- Scales with a negative Y component (e.g. `(0.001f, -0.001f, 0.001f)`) are used throughout `gameSetUp()` to flip the Y-axis on imported models — this is intentional for models whose coordinate system doesn't match OpenGL's.
- The `normalMatrix` is `transpose(inverse(modelMatrix))`, computed CPU-side in `Renderer::DrawTriangles()` and uploaded per draw call. If adding instancing or batching, move this computation appropriately.
- There is no asset management system — `Shader` objects are heap-allocated by the `Game` class and deleted in `cleanup()`. Models loaded via Assimp are wrapped immediately into `SceneNode` trees by `ModelLoader`.

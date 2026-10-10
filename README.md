# 3D OpenGL Renderer
<img width="920" height="509" alt="Screenshot 2026-10-10 123732" src="https://github.com/user-attachments/assets/d934224b-fbd7-4225-acba-b58ba0fde5c2" />

After taking a class on Computer Graphics, I decided that I wanted to explore more on lower level graphics. My previous projects were on an older version of WebGL and I wanted to use Modern OpenGL and write my project in C++ which is what most of the industry uses.
Another goal of the project was to make the code a lot more reusable and modular in design. 
I wanted to make sure that I can have an environment that I can easily add more features and explore other graphics topics, such as Procedural Generation.

## Demos
### Simple 3D Day Scene
[<img src="https://img.youtube.com/vi/lYOe-wyHsb4/hqdefault.jpg" width="600" height="300"
/>](https://www.youtube.com/embed/lYOe-wyHsb4)

### Shadow Mapping
<!-- TODO: add demo video/screenshot -->
*Demo coming soon.*

Directional, spot, and point lights can all cast real-time shadows. Lights can be toggled to cast shadows from the Lighting panel.

### Normal Mapping
<!-- TODO: add demo video/screenshot -->
*Demo coming soon.*

Side-by-side of a model with and without its normal map, showing surface detail picked up by the lighting.

### Editor UI
<!-- TODO: add demo video/screenshot -->
*Demo coming soon.*

Building a scene at runtime: importing models, adding primitives and lights, and tweaking materials and transforms through the ImGui panels.

### Frustum Culling and Render Queue
<!-- TODO: add demo video/screenshot -->
*Demo coming soon.*

The Performance panel showing draw calls dropping as objects leave the camera's view.

### Debug Overlay
<!-- TODO: add demo video/screenshot -->
*Demo coming soon.*

Light gizmos and bounding boxes drawn over the scene, toggled from the Renderer Settings panel.

I will be adding additional demos in this section when I get to it.

## Technical Implementation
- **Language**: C++17 with Modern OpenGL (4.6 core profile context)
- **Architecture**: Component-based scene graph system
- **Shaders**: Custom GLSL vertex and fragment shaders

### Versions
| Component | Version | Purpose |
|---|---|---|
| OpenGL | 4.6 core (shaders target GLSL 3.30 / 4.00) | Graphics API |
| GLFW | 3.4.0 | Window management and input |
| GLEW | 2.1.0 (static) | OpenGL extension loading |
| GLM | 1.0.1 | Mathematics |
| Assimp | prebuilt `assimp-vc143-mt` | Model loading |
| Dear ImGui | 1.92.8 | Editor UI |
| stb_image | 2.30 | Texture loading |
| Visual Studio | 2022 (MSVC v143 toolset) | Compiler / IDE |
| Windows SDK | 10.0 | Platform |

All libraries are included pre-built for x64 in the `Dependencies/` folder, so nothing needs to be installed separately.

## Building and Running
### Requirements
- Windows 10 or 11 (x64)
- Visual Studio 2022 with the **Desktop development with C++** workload
- A GPU and driver that support OpenGL 4.6

### Visual Studio
1. Clone the repository:
   ```
   git clone https://github.com/PreethamOnFire/OpenGL-Learning.git
   ```
2. Open `GLFW.sln` in Visual Studio 2022.
3. Select the **Debug|x64** or **Release|x64** configuration.
4. In the project's **Properties → Debugging → Working Directory**, set it to `$(ProjectDir)` (the inner `GLFW/` folder) so shaders (`src/Shaders/...`) and assets (`assets/...`) are found.
5. Build and run (F5).

### Command Line
From a **Developer Command Prompt for VS 2022**:
```
msbuild GLFW.sln /p:Configuration=Release /p:Platform=x64
cd GLFW
..\x64\Release\GLFW.exe
```
The build copies `assimp-vc143-mt.dll` next to the executable automatically. The executable must be run from the inner `GLFW/` folder, since shader and asset paths are relative to it.

### Controls
- **WASD**: Move
- **Left Shift**: Move faster
- **Mouse**: Look around (while the cursor is captured)
- **Left Click**: Capture the cursor
- **Escape**: Release the cursor to use the UI

## Features
- **Model Loading**: Support for OBJ and GLTF formats via Assimp integration, including embedded GLTF/GLB textures
- **Advanced Lighting**: Blinn-Phong illumination model with support for up to 16 lights:
  - Directional lights (sun/moon)
  - Point lights with attenuation
  - Spot lights with cone angles
- **Shadow Mapping**:
  - Directional and spot light shadows rendered into a shared shadow map array with hardware PCF
  - Omnidirectional point light shadows using a cubemap array
- **Normal Mapping**: Per-vertex tangents (loaded from Assimp or generated for primitives) build a TBN matrix so tangent-space normal maps light correctly, including mirrored UVs and negatively scaled models
- **Material System**: Diffuse, specular, and normal map support, with materials and shader pipelines stored in shared libraries and referenced by name
- **Shader Pipelines**: Each pipeline owns its GL render state (depth test, blending, face culling), applied when its material is bound
- **Frustum Culling**: Models outside the camera's view are skipped using bounding boxes
- **Render Queue**: Draw calls are sorted by pipeline and material to reduce state changes
- **Scene Management**: Hierarchical scene graph with transform inheritance
- **Debug Overlay**: A separate debug pass, run after the main forward pass, that draws light gizmos (point light spheres, spot light cones, directional light arrows) and model/mesh bounding boxes. All lines for a frame are batched into a single vertex buffer and drawn in one call, with toggles in the Renderer Settings panel
- **Editor UI**: ImGui panels for the scene, model inspector, materials, lighting, camera, renderer settings (wireframe, VSync), and performance stats
- **Runtime Model Importing**: Add models from disk or spawn cubes, spheres, and planes through the UI
- **Camera System**: Smooth FPS controls with mouse look
- **Skybox Rendering**: 360-degree environment mapping
- **Extensible Design**: Modular architecture for easy feature additions

### Usage Example
Setting up a complete 3D scene with lighting and models requires just a few lines:
<img width="1438" height="1109" alt="Screenshot 2025-09-05 173028" src="https://github.com/user-attachments/assets/c0c2e92a-b7c5-41ff-9b83-b6e999fe1ffd" />

## Features/Concepts for the Future
I wanted to make this project to create a platform for me to explore any other graphics concepts that interest me. 
So here is the list of other things I'm looking to add.
- Procedural Generation
- Rendering liquids

# Previous Graphics Projects
Here are the previous projects that I worked on that led me to this point. They go from my first WebGL project to my most recent project built on a higher-level library called Three.js.

## Blocky Toothless
<img width="690" height="652" alt="Screenshot 2025-09-05 165333" src="https://github.com/user-attachments/assets/01a6522e-3ed8-4ce2-928d-4d386f6f17fb" />

Link: https://github.com/PreethamOnFire/CSE160-Assignment2

## Blocky Severance Office
<img width="2055" height="1171" alt="Screenshot 2025-09-05 165843" src="https://github.com/user-attachments/assets/c4564a6b-708f-4879-9f76-674f3f2ee5a8" />
Link: https://github.com/PreethamOnFire/CSE160-Assignment3

## Severance Office + Lighting Expiraments 
<img width="2094" height="1213" alt="Screenshot 2025-09-05 170435" src="https://github.com/user-attachments/assets/768e37cf-eba1-43f3-8b86-3d9b4ea2ea1b" />
Link: https://github.com/PreethamOnFire/CSE160-Assignment4

## Explore the Solar System in the Normandy
<img width="2799" height="1308" alt="Screenshot 2025-09-05 170740" src="https://github.com/user-attachments/assets/5242d8c9-b454-4d7b-9469-9a763ea701e8" />
Link: https://github.com/PreethamOnFire/CSE160-Assignment5

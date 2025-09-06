# 3D OpenGL Renderer
<img width="2247" height="1362" alt="Screenshot 2025-09-05 171245" src="https://github.com/user-attachments/assets/6a75354a-5581-4531-9186-d0a331724aef" />
After taking a class on Computer Graphics, I decided that I wanted to explore more on lower level graphics. My previous projects were on an older version of WebGL and I wanted to use Modern OpenGL and write my project in C++ which is what most of the industry uses.
Another goal of the project was to make the code a lot more reusable and modular in design. 
I wanted to make sure that I can have an environment that I can easily add more features and explore other graphics topics, such as Procedural Generation.

## Demos
I will be adding additional demos in this section when I get to it.

## Technical Implementation
- **Language**: C++ with Modern OpenGL (4.0+)
- **Libraries**: 
  - GLFW (Window Management)
  - GLEW (OpenGL Extension Loading)
  - Assimp (Model Loading)
  - GLM (Mathematics)
  - stb_image (Texture Loading)
- **Architecture**: Component-based scene graph system
- **Shaders**: Custom GLSL vertex and fragment shaders

## Features
- **Model Loading**: Support for OBJ and GLTF formats via Assimp integration
- **Advanced Lighting**: Phong illumination model with support for:
  - Directional lights (sun/moon)
  - Point lights with attenuation
  - Spot lights with cone angles
- **Material System**: Diffuse, specular, and normal map support
- **Scene Management**: Hierarchical scene graph with transform inheritance
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
- Shadows

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

# LostCity

A small, fun OpenGL project written in modern C++ (OpenGL 3.3 core).
Explore a lit, textured room in first person while experimenting with
real-time rendering techniques.

## Features

- First-person free camera (keyboard + mouse, scroll to zoom)
- Procedurally generated room (`MeshFactory::room`) with separate wall and floor textures
- Up to **4 point lights** (`Renderer::kMaxLights`)
- **Phong / Blinn-Phong** shading, switchable at runtime
- **Gamma correction** switchable at runtime (textures are loaded in both linear and sRGB variants and cached)
- Clean frame loop: `tickTime → processInput → update → render`
- Automatic handling of window resize and HiDPI framebuffers

## Controls

| Input          | Action                              |
| -------------- | ----------------------------------- |
| `W` `A` `S` `D`| Move the camera                     |
| Mouse          | Look around                         |
| Scroll wheel   | Zoom (changes the field of view)    |
| `B`            | Toggle Phong / Blinn-Phong          |
| `G`            | Toggle gamma correction             |
| `Esc`          | Quit                                |

## Requirements

- A C++17 (or newer) compiler
- [CMake](https://cmake.org/) 3.10+
- A GPU/driver supporting **OpenGL 3.3**
- [GLFW](https://www.glfw.org/) (window, input, context)
- [GLAD](https://glad.dav1d.de/) (OpenGL loader)
- [GLM](https://github.com/g-truc/glm) (math)

Third-party headers live in `dependencies/include/`.

## Build & run

```bash
cmake -S . -B build
cmake --build build -j
```

Then launch the executable from the build directory, for example:

```bash
./build/LostCity
```

> The executable name and location may differ depending on your `CMakeLists.txt`.

## Project structure

```text
LostCity/
├── src/                   # Source code and project headers
├── dependencies/include/  # Third-party headers (GLAD, GLM, ...)
└── resources/             # Textures, models, materials, shaders...
```

### Code overview

| Class / module | Role                                                                                          |
| -------------- | --------------------------------------------------------------------------------------------- |
| `Game`         | Owns the window, the main loop, input callbacks (GLFW) and the scene                          |
| `Renderer`     | Hides low-level OpenGL: shader, uniforms, GL state, lights, texture loading and caching       |
| `Mesh`         | Geometry + textures, drawn through the renderer                                               |
| `MeshFactory`  | Builds meshes procedurally (e.g. the room)                                                    |
| `Camera`       | First-person camera (view matrix, movement, zoom)                                             |
| `Shader`       | Compiles and wraps the GLSL program                                                           |

Typical usage from the game code:

```cpp
renderer.beginFrame(camera, projection);
renderer.draw(mesh, modelMatrix);   // as many times as needed
```

## Roadmap

- [ ] Game logic in `Game::update` (entities, physics, AI...)
- [ ] More lights and materials

## Credits

Rendering concepts and helper structure inspired by
[LearnOpenGL](https://learnopengl.com/).

# cube-opengl
Cube in Modern OpenGL - Wireframe (because wireframe is cool) Whats even cooler is that its in C

I haven't deliberated over the code that much, meaning im not sure if it leaks memory, if I had to guess i'd say probably not though

Also, why is it so hard to find OpenGL projects in C? It's genuinely disheartening.

![](https://github.com/usesc/cube-opengl/blob/main/res/ogl.gif)

![](https://github.com/usesc/cube-opengl/blob/main/res/ogl.png)

Want to remove wireframe and have a regular filled cube? Just erase line 185!

DEPENDENCIES:
- Modern C compiler (GCC, Clang, etc)
- Modern C Standard Library
- Somewhat Modern Hardware (needs to support OpenGL)
- CGLM
- GLFW
- GLEW
- Make (Build system)

I VERY STRONGLY RECOMMEND USING YOUR OPERATING SYSTEMS PACKAGE MANAGER, IF YOUR ON WINDOWS JUST USE MSYS2 MINGW.

Anyway heres how to install cglm, glew, glfw for ubuntu (apt)

oh and also install libopengl
```bask
sudo apt install libopengl-dev libglew-dev libglfw3-dev libcglm-dev
```

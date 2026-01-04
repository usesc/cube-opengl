# cube-opengl v2
Cube in Modern OpenGL

UPDATE: This code is bad, I made a better version, I am too lazy to post it

![](https://github.com/usesc/cube-opengl/blob/main/res/demo3.gif)

![](https://github.com/usesc/cube-opengl/blob/main/res/ogl.png)

DEPENDENCIES:
- Modern C compiler (GCC, Clang, etc)
- Modern C Standard Library
- Somewhat Modern Hardware (needs to support OpenGL)
- CGLM
- GLFW
- GLEW
- Make (Build system)

Anyway heres how to install cglm, glew, glfw for ubuntu (apt)

also install libopengl
```bash
sudo apt install libopengl-dev libglew-dev libglfw3-dev libcglm-dev
```

Currently I get a libdecor error when i try to run it, run this in the console if this happens:
```bash
XDG_SESSION_TYPE=x11
```

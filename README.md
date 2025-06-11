# cube-opengl
Cube in Modern OpenGL - Wireframe (because wireframe is cool) Whats even cooler is that its in C

I haven't deliberated over the code that much, meaning im not sure if it leaks memory, if I had to guess i'd say it probably doesnt.

Also, why is it so hard to find OpenGL projects in C? It's genuinely disheartening.

![](https://github.com/usesc/cube-opengl/blob/main/res/ogl.gif)

(Colors more vibrant IRL, i used a low quality webm to gif converter online)

![](https://github.com/usesc/cube-opengl/blob/main/res/ogl.png)

Want to remove wireframe and have a regular filled cube? Just erase this line!
```c
glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
```

DEPENDENCIES:
- Modern C compiler (GCC, Clang, etc)
- Modern C Standard Library
- Somewhat Modern Hardware (needs to support OpenGL)
- CGLM
- GLFW
- GLEW
- Make (Build system)

I very strongly recommend using your operating systems package manager, if your on windows just use MSYS2 MinGW. Building libraries is just too much of a hassle nowadays, especially since everything requires Cmake.

Anyway heres how to install cglm, glew, glfw for ubuntu (apt)

also install libopengl
```bask
sudo apt install libopengl-dev libglew-dev libglfw3-dev libcglm-dev
```

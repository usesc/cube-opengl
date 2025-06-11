#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

uniform mat4 u_MVP;

out vec3 vColor;

void main() {
  vColor = aColor;
  gl_Position = u_MVP * vec4(aPos, 1.0);
}

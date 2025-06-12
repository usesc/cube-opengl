#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "vertices.c"

// XDG_SESSION_TYPE=x11

const int window_width = 800;
const int window_height = 600;
const char *window_name = "my opengl";

const int virtual_width = 2560;
const int virtual_height = 1440;

const char *vertex_shader_file = "vertex.glsl";
const char *fragment_shader_file = "fragment.glsl";

const float inc = 0.02f;

uint32_t mode_i = 0;

bool is_culling = false;

float retmin(float x, float y) {
  return x < y ? x : y;
}

static inline uint32_t value_wheel(uint32_t x, uint32_t y) {
  return x % (y+1);
}

static char* read_file(const char* path) {
  FILE* f = fopen(path, "rb");
  if (!f) { 
    fprintf(stderr, "Cannot open %s\n", path); 
    exit(1); 
  }
  fseek(f, 0, SEEK_END);
  long sz = ftell(f);
  rewind(f);
  char* buf = malloc(sz + 1);
  fread(buf, 1, sz, f);
  buf[sz] = '\0';
  fclose(f);
  return buf;
}

GLuint compile_shader(GLenum type, const char* path) {
  char* src = read_file(path);
  GLuint s = glCreateShader(type);
  glShaderSource(s, 1, (const char**)&src, NULL);
  glCompileShader(s);
  free(src);
  GLint ok;
  glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    char log[512];
    glGetShaderInfoLog(s, 512, NULL, log);
    fprintf(stderr, "Shader compile failed (%s):\n%s\n", path, log);
    glDeleteShader(s);
  }
  return s;
}

void mode(uint32_t mde) {
  uint32_t md = value_wheel(mde, 2);
  switch (md) {
    case 0:
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      return;
    case 1:
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      return;
    case 2:
      glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
      return;
    default:
      return;
  }
}

void culler(bool state) {
  state ? (glEnable(GL_CULL_FACE), glCullFace(GL_BACK)) : glDisable(GL_CULL_FACE);
}

void magic_viewport(GLFWwindow *w, int virtual_width, int virtual_height) {
  int window_width, window_height;
  glfwGetFramebufferSize(w, &window_width, &window_height);

  float scale_x = (float)window_width / virtual_width;
  float scale_y = (float)window_height / virtual_height;
  float scale = retmin(scale_x, scale_y);

  int render_width = (int)(virtual_width * scale);
  int render_height = (int)(virtual_height * scale);

  int offset_x = (window_width - render_width) / 2;
  int offset_y = (window_height - render_height) / 2;

  glViewport(offset_x, offset_y, render_width, render_height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_W) {
      mode_i++;
      mode(mode_i);
    }
    if (key == GLFW_KEY_C) {
      is_culling = !is_culling;
      culler(is_culling);
    }
  }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  magic_viewport(window, virtual_width, virtual_height);
}

int main(void) {
  if (!glfwInit()) return 1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(window_width, window_height, window_name, NULL, NULL);
  if (!window) { 
    glfwTerminate(); 
    return 1; 
  }
  glfwMakeContextCurrent(window);
  if (glewInit() != GLEW_OK) return 1;

  glfwSwapInterval(1);

  float aspect_ratio = (float)virtual_width / (float)virtual_height;

  GLuint prog = glCreateProgram();
  GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_shader_file);
  GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_file);
  glAttachShader(prog, vs);
  glAttachShader(prog, fs);
  glLinkProgram(prog);
  glDeleteShader(vs);
  glDeleteShader(fs);
  glUseProgram(prog);

  GLint mvp_loc = glGetUniformLocation(prog, "u_MVP");
  if (mvp_loc == -1) {
    fprintf(stderr, "Warning: uniform 'u_MVP' not found in shader\n");
  }

  GLuint vao, vbo, ebo;
  glCreateVertexArrays(1, &vao);
  glCreateBuffers(1, &vbo);
  glCreateBuffers(1, &ebo);

  glNamedBufferData(vbo, sizeof(vertices_color_mash), vertices_color_mash, GL_STATIC_DRAW);
  glNamedBufferData(ebo, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

  glVertexArrayVertexBuffer(vao, 0, vbo, 0, 6 * sizeof(float));
  glVertexArrayElementBuffer(vao, ebo);

  glEnableVertexArrayAttrib(vao, 0);
  glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribBinding(vao, 0, 0);

  glEnableVertexArrayAttrib(vao, 1);
  glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
  glVertexArrayAttribBinding(vao, 1, 0);

  mat4 model, view, proj, mvp;
  vec3 eye = {0.0f, 0.0f, 3.0f};
  vec3 center = {0.0f, 0.0f, 0.0f};
  vec3 up = {0.0f, 1.0f, 0.0f};

  glEnable(GL_DEPTH_TEST);

  float scale = 1.0f;
  float anglex = 0.0f, angley = 0.0f, anglez = 0.0f;

  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  while (!glfwWindowShouldClose(window)) {
    magic_viewport(window, virtual_width, virtual_height);

    glfwPollEvents();
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    anglex -= inc;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  anglex += inc;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  angley += inc;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) angley -= inc;  

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) scale += inc;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) scale -= inc;  

    glm_mat4_identity(model);
    glm_rotate(model, anglex, (vec3){1.0f, 0.0f, 0.0f});
    glm_rotate(model, angley, (vec3){0.0f, 1.0f, 0.0f});
    glm_rotate(model, anglez, (vec3){0.0f, 0.0f, 1.0f});
    glm_scale(model, (vec3){scale, scale, scale});

    glm_lookat(eye, center, up, view);
    glm_perspective(glm_rad(45.0f), aspect_ratio, 0.1f, 100.0f, proj);
    glm_mat4_mul(proj, view, mvp);
    glm_mat4_mul(mvp, model, mvp);

    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)mvp);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, sizeof(cube_indices)/sizeof(unsigned int), GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
  }

  glDeleteProgram(prog);
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
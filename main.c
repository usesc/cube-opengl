#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const int window_width = 800;
const int window_height = 600;
const char *window_name = "my opengl";

const int virtual_width = 1600;
const int virtual_height = 900;

const char *vertex_shader_file = "vertex.glsl";
const char *fragment_shader_file = "fragment.glsl";

float retmin(float x, float y) {
  return x < y ? x : y;
}

static char* readFile(const char* path) {
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

// Compile shader, exit on failure
static GLuint compileShader(GLenum type, const char* path) {
  char* src = readFile(path);
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
    exit(1);
  }
  return s;
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

  float aspect_ratio = (float)virtual_width / (float)virtual_height;

  GLuint prog = glCreateProgram();
  GLuint vs = compileShader(GL_VERTEX_SHADER, vertex_shader_file);
  GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragment_shader_file);
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

  float vertices[] = {
    // (X, Y, Z) (R, G, B)
    // front face - triangle 1
    -0.5f, -0.5f,  0.5f,  1, 0, 0,
     0.5f, -0.5f,  0.5f,  0, 1, 0,
     0.5f,  0.5f,  0.5f,  0, 0, 1,
    // front face - triangle 2
    -0.5f, -0.5f,  0.5f,  1, 0, 0,
     0.5f,  0.5f,  0.5f,  0, 0, 1,
    -0.5f,  0.5f,  0.5f,  1, 1, 0,
    // back face - triangle 3
    -0.5f, -0.5f, -0.5f,  1, 0, 1,
     0.5f,  0.5f, -0.5f,  0, 1, 1,
     0.5f, -0.5f, -0.5f,  0, 1, 0,

    -0.5f, -0.5f, -0.5f,  1, 0, 1,
    -0.5f,  0.5f, -0.5f,  1, 1, 1,
     0.5f,  0.5f, -0.5f,  0, 1, 1,

    // Left face
    -0.5f, -0.5f, -0.5f,  1, 0, 1,
    -0.5f, -0.5f,  0.5f,  1, 0, 0,
    -0.5f,  0.5f,  0.5f,  1, 1, 0,

    -0.5f, -0.5f, -0.5f,  1, 0, 1,
    -0.5f,  0.5f,  0.5f,  1, 1, 0,
    -0.5f,  0.5f, -0.5f,  1, 1, 1,

    // Right face
     0.5f, -0.5f, -0.5f,  0, 1, 0,
     0.5f,  0.5f,  0.5f,  0, 0, 1,
     0.5f, -0.5f,  0.5f,  0, 1, 0,

     0.5f, -0.5f, -0.5f,  0, 1, 0,
     0.5f,  0.5f, -0.5f,  0, 1, 1,
     0.5f,  0.5f,  0.5f,  0, 0, 1,

    // Top face
    -0.5f,  0.5f, -0.5f,  1, 1, 1,
    -0.5f,  0.5f,  0.5f,  1, 1, 0,
     0.5f,  0.5f,  0.5f,  0, 0, 1,

    -0.5f,  0.5f, -0.5f,  1, 1, 1,
     0.5f,  0.5f,  0.5f,  0, 0, 1,
     0.5f,  0.5f, -0.5f,  0, 1, 1,

    // Bottom face
    -0.5f, -0.5f, -0.5f,  1, 0, 1,
     0.5f, -0.5f,  0.5f,  0, 1, 0,
    -0.5f, -0.5f,  0.5f,  1, 0, 0,

    -0.5f, -0.5f, -0.5f,  1, 0, 1,
     0.5f, -0.5f, -0.5f,  0, 1, 0,
     0.5f, -0.5f,  0.5f,  0, 1, 0,
  };

  GLuint vao, vbo;
  glCreateVertexArrays(1, &vao);
  glCreateBuffers(1, &vbo);
  glNamedBufferData(vbo, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexArrayVertexBuffer(vao, 0, vbo, 0, 6 * sizeof(float));

  glEnableVertexArrayAttrib(vao, 0);
  glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribBinding(vao, 0, 0);

  glEnableVertexArrayAttrib(vao, 1);
  glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
  glVertexArrayAttribBinding(vao, 1, 0);

  // glClearColor(30.0f / 255.0f, 30.0f / 255.0f, 30.0f / 255.0f, 1.0f);

  mat4 model, view, proj, mvp;
  vec3 eye = {0.0f, 0.0f, 3.0f};
  vec3 center = {0.0f, 0.0f, 0.0f};
  vec3 up = {0.0f, 1.0f, 0.0f};
  glm_lookat(eye, center, up, view);

  glEnable(GL_DEPTH_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  float scale = 1.0f;
  float anglex = 0.0f, angley = 0.0f, anglez = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    magic_viewport(window, virtual_width, virtual_height);

    glm_mat4_identity(model);
    glm_scale(model, (vec3){scale, scale, scale});

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) anglex -= 0.02f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) anglex += 0.02f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) angley += 0.02f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) angley -= 0.02f;

    glm_rotate(model, anglex, (vec3){1.0f, 0.0f, 0.0f});
    glm_rotate(model, angley, (vec3){0.0f, 1.0f, 0.0f});
    glm_rotate(model, anglez, (vec3){0.0f, 0.0f, 1.0f});

    glm_lookat(eye, center, up, view);
    glm_perspective(glm_rad(45.0f), aspect_ratio, 0.1f, 100.0f, proj);
    glm_mat4_mul(proj, view, mvp);
    glm_mat4_mul(mvp, model, mvp);

    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)mvp);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(prog);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteProgram(prog);
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}


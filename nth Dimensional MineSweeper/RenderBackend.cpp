/*********************************************************************
 * @file   RenderBackend.cpp
 * @brief
 *
 * @author Lorenzo St. Luce(lorenzo.stluce)
 * @date   November 2023
 *********************************************************************/

#include "RenderBackend.h"
#include <ext.hpp>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <thread>
typedef struct Vertex
{
  glm::vec4 pos;
  glm::vec2 tex;
  glm::vec2 buff;
}Vertex;



static std::vector<GLchar>& loadFile(const char* fileName)
{
  static std::vector<GLchar> file;
  file.clear();
  FILE* f;
  fopen_s(&f, fileName, "rb");
  if (f == nullptr)
    throw std::runtime_error("File not opened correctly, likely bad path");
  size_t length = 0;
  fseek(f, 0, SEEK_END);
  length = ftell(f);
  file.resize(length + 1);
  fseek(f, 0, 0);
  fread(file.data(), length, 1, f);
  fclose(f);
  return file;
}
GLuint CreateShader(GLenum type, const char* filepath)
{
  auto& frag = loadFile(filepath);
  GLuint result = glCreateShader(type);
  auto p = frag.data();
  glShaderSource(result, 1, &p, nullptr);
  glCompileShader(result);

  int linkok = 0;
  glGetShaderiv(result, GL_COMPILE_STATUS, &linkok);
  if (linkok == 0)
  {
    char buffer[1000];
    GLsizei len;
    glGetShaderInfoLog(result, _countof(buffer), &len, buffer);
    throw std::runtime_error(buffer);
  }

  return result;
}

Renderer::Renderer()
  : _window(nullptr)
  , _projectionMatrix(glm::identity<glm::mat4x4>())
  , _objectMatrix(glm::identity<glm::mat4x4>())
  , _VAO(0)
  , _VBO(0)
  , _shader(0)
  , _projectionBinding(0)
  , _objectBinding(0)
  , _textured(0)
  , _color(0)
{
  SDL_Init(SDL_INIT_VIDEO);
  const char* title = "nth Dimensional MineSweeper";
  int width = 1280, height = 720;
  _windowSize = { width, height };
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  _window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED, width, height,
    SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

  SDL_GLContext context = SDL_GL_CreateContext(_window);
  
  int state = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress));
  SDL_GL_MakeCurrent(_window, context);
  SDL_GL_SetSwapInterval(1);
  glClearColor(204.f / 255.f, 154.f / 255.f, 254.f / 255.f, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapWindow(_window);
  glClear(GL_COLOR_BUFFER_BIT);
  
  GLuint fragment = CreateShader(GL_FRAGMENT_SHADER, "./Managed/Shaders/main.frag");
  GLuint vert = CreateShader(GL_VERTEX_SHADER, "./Managed/Shaders/main.vert");
  _shader = glCreateProgram();

  glAttachShader(_shader, fragment);
  glAttachShader(_shader, vert);

  constexpr int posLoc = 0;
  constexpr int texLoc = 4;

  glBindAttribLocation(_shader, posLoc, "pos");
  glBindAttribLocation(_shader, texLoc, "texcoord");

  glLinkProgram(_shader);
  glDeleteShader(fragment);
  glDeleteShader(vert);
  GLint linkok = 0;
  glGetProgramiv(_shader, GL_LINK_STATUS, &linkok);
  if (linkok == 0)
  {
    char buffer[1000];
    GLsizei len;
    glGetProgramInfoLog(_shader, _countof(buffer), &len, buffer);
    std::cerr << buffer << std::endl;
    throw std::runtime_error(buffer);
  }
  glUseProgram(_shader);
  glGenVertexArrays(1, &_VAO);

  glGenBuffers(1, &_VBO);
  assert(!glGetError());

  glBindVertexArray(_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, _VBO);
  glEnableVertexAttribArray(posLoc);
  glEnableVertexAttribArray(texLoc);
  assert(!glGetError());
  glVertexAttribPointer(
    posLoc,
    4,
    GL_FLOAT,
    GL_FALSE,
    static_cast<GLsizei>(sizeof(Vertex)),
    reinterpret_cast<void*>(0));
  assert(!glGetError());
  glVertexAttribPointer(
    texLoc,
    2,
    GL_FLOAT,
    GL_FALSE,
    static_cast<GLsizei>(sizeof(Vertex)),
    reinterpret_cast<void*>(sizeof(Vertex::pos)));

  _color = glGetUniformLocation(_shader, "color");
  _objectBinding = glGetUniformLocation(_shader, "objectMatrix");
  _projectionBinding = glGetUniformLocation(_shader, "screenMatrix");
  _textured = glGetUniformLocation(_shader, "textured");
  glUniform1i(_textured, 0);
  glViewport(0, 0, width, height);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  assert(!glGetError());
  glBindVertexArray(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glm::mat4x4 camMat = mainCamera.GetMatrix();
  glm::vec3 camPos = camMat * glm::vec4(0, 0, 0, 1);
  glm::vec3 lookatPos = camMat * glm::vec4(0, 0, 2, 1);
  _projectionMatrix = glm::perspective(120.0f, _windowSize.x / _windowSize.y, 1.0f, 1500.0f) * glm::lookAt(camPos, lookatPos, glm::vec3(0, 1, 0)) * camMat;
  glUniformMatrix4fv(_projectionBinding, 1, false, &_projectionMatrix[0][0]);
  assert(!glGetError());
}

void Renderer::DrawRect()
{
  glUseProgram(_shader);
  glBindVertexArray(_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, _VBO);
  const Vertex mesh[4] = {
    {{-.5f, -.5f, 0, 1},{0, 1} },
    { {-.5f, .5f, 0, 1},{0, 0} },
    {{.5f, .5f, 0, 1}, {1,0}},
    {{.5f, -.5f, 0, 1}, {1, 1}} };
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, mesh, GL_STATIC_DRAW);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  assert(!glGetError());
}

void Renderer::SetColor(glm::vec4 color)
{
  glUseProgram(_shader);
  glUniform4fv(_color, 1, &color[0]);
  assert(!glGetError());
}

void Renderer::SetMatrix(glm::vec3 pos, glm::vec3 scale)
{
  glUseProgram(_shader);
  _objectMatrix = glm::scale(glm::identity<glm::mat4>(), scale);
  _objectMatrix = glm::translate(_objectMatrix, pos * glm::vec3( - 1, 1 , 1));
  glUniformMatrix4fv(_objectBinding, 1, false, &_objectMatrix[0][0]);
  assert(!glGetError());

}

void Renderer::Update()
{
  glUseProgram(_shader);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  assert(!glGetError());
  SDL_GL_SwapWindow(_window);
  //SDL_UpdateWindowSurface(_window);
  assert(!glGetError());
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_Event ev;
  while (SDL_PollEvent(&ev))
  {
    switch (ev.type)
    {
    case SDL_QUIT:
      running = false;
      break;
    }


  }
  glm::mat4x4 camMat = mainCamera.GetMatrix();
  glm::vec3 camPos = camMat * glm::vec4(0, 0, 0, 1);
  glm::vec3 lookatPos = camMat * glm::vec4(0, 0, 2, 1);

  _projectionMatrix = glm::perspective(120.0f, _windowSize.x / _windowSize.y, 1.0f, 1500.0f) * glm::lookAt(camPos, lookatPos, glm::vec3(0, 1, 0)) * camMat;
  glUniformMatrix4fv(_projectionBinding, 1, false, &_projectionMatrix[0][0]);

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

}

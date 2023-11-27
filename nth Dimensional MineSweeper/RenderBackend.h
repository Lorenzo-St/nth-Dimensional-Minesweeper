/*********************************************************************
 * @file   RenderBackend.h
 * @brief  
 * 
 * @author Lorenzo St. Luce(lorenzo.stluce)
 * @date   November 2023
 *********************************************************************/
#pragma once
#include <glad.h>
#include <glm.hpp>
#include <SDL.h>

#include "Camera.h"

class Renderer 
{
public:
  bool running = true;
  Renderer();
  
  void DrawRect();
  void SetColor(glm::vec4 color);
  void SetMatrix(glm::vec3 pos, glm::vec3 scale);
  
  void Update();

private:

  Camera mainCamera;

  // Shader
  GLuint _shader;
  // Uniforms
  GLuint _objectBinding, _projectionBinding, _textured, _color;
  // Buffers
  GLuint _VAO, _VBO;
  // Window handle
  SDL_Window* _window;
  // Window Size
  glm::vec2 _windowSize;
  // Matricies
  glm::mat4x4 _objectMatrix, _projectionMatrix;
};

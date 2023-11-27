/*****************************************************************
 * @file   Camera.hpp
 * @brief  Header for the Camera class
 * @author Wei-No Wu (weino.wu)
 * @date   November 2023
 *
 * @copyright © 2023 DigiPen (USA) Corporation.
 *****************************************************************/

#include <ext\matrix_clip_space.hpp>
#include <ext\matrix_transform.hpp>
#include <glm.hpp>
typedef struct Camera
{
private:
  bool dirty = true;
  float rotation = 0;
  float zoom = 1;
  glm::vec3 position = { 0, 0, 0 };
  glm::mat4x4 matrix;

public:
  glm::vec2 Position()
  {
    return glm::vec2(position);
  }
  glm::mat4x4 GetMatrix()
  {
    if (dirty)
    {
      glm::mat4x4 scaled =
        glm::scale(glm::identity<glm::mat4x4>(), glm::vec3(zoom, zoom, zoom));
      glm::mat4x4 rot = glm::rotate(scaled, glm::radians(rotation), { 0, 0, 1 });
      matrix = glm::translate(rot, position);
      dirty = false;
    }
    return matrix;
  }
  void moveCamera(glm::vec3 newPos)
  {
    position = newPos;
    dirty = true;
  }
  void moveCamera(glm::vec2 newPos)
  {
    position = glm::vec3(newPos, position.z);
    dirty = true;
  }
  void setZoom(float f)
  {
    zoom = f;
    dirty = true;
  };
  void rotateCamera(float newRot)
  {
    rotation = newRot;
    dirty = true;
  }
} Camera;
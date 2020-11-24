#pragma once

#include <water_simulation/render/renderable.hpp>

#include <glm/glm.hpp>

namespace component
{
   struct transform
   {
      glm::mat4 translate;
      glm::mat4 scale;
   };

   struct render
   {
      renderable* p_mesh;
      glm::vec3 colour;
   };

   struct box_collider
   {
      glm::vec3 center;
      glm::vec3 half_size;
   };
} // namespace component

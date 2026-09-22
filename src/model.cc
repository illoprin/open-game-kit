#include "model.hpp"

Geometry Geometry::CreateCube(
  glm::vec3 size
) {
  std::vector<ModelVertex> verts {
    // front face (z = size.z/2)
    {{-size.x / 2, -size.y / 2, size.z / 2},  {0.0f, 0.0f, 1.0f},  {0.0f,   0.0f  }},
    {{size.x / 2, -size.y / 2, size.z / 2},   {0.0f, 0.0f, 1.0f},  {size.x, 0.0f  }},
    {{size.x / 2, size.y / 2, size.z / 2},    {0.0f, 0.0f, 1.0f},  {size.x, size.y}},
    {{-size.x / 2, size.y / 2, size.z / 2},   {0.0f, 0.0f, 1.0f},  {0.0f,   size.y}},

    // back face (z = -size.z/2)
    {{-size.x / 2, -size.y / 2, -size.z / 2}, {0.0f, 0.0f, -1.0f}, {size.x, 0.0f  }},
    {{-size.x / 2, size.y / 2, -size.z / 2},  {0.0f, 0.0f, -1.0f}, {size.x, size.y}},
    {{size.x / 2, size.y / 2, -size.z / 2},   {0.0f, 0.0f, -1.0f}, {0.0f,   size.y}},
    {{size.x / 2, -size.y / 2, -size.z / 2},  {0.0f, 0.0f, -1.0f}, {0.0f,   0.0f  }},

    // top face (y = size.y/2)
    {{-size.x / 2, size.y / 2, -size.z / 2},  {0.0f, 1.0f, 0.0f},  {0.0f,   size.z}},
    {{-size.x / 2, size.y / 2, size.z / 2},   {0.0f, 1.0f, 0.0f},  {0.0f,   0.0f  }},
    {{size.x / 2, size.y / 2, size.z / 2},    {0.0f, 1.0f, 0.0f},  {size.x, 0.0f  }},
    {{size.x / 2, size.y / 2, -size.z / 2},   {0.0f, 1.0f, 0.0f},  {size.x, size.z}},

    // bottom face (y = -size.y/2)
    {{-size.x / 2, -size.y / 2, -size.z / 2}, {0.0f, -1.0f, 0.0f}, {0.0f,   0.0f  }},
    {{size.x / 2, -size.y / 2, -size.z / 2},  {0.0f, -1.0f, 0.0f}, {size.x, 0.0f  }},
    {{size.x / 2, -size.y / 2, size.z / 2},   {0.0f, -1.0f, 0.0f}, {size.x, size.z}},
    {{-size.x / 2, -size.y / 2, size.z / 2},  {0.0f, -1.0f, 0.0f}, {0.0f,   size.z}},

    // right face (x = size.x/2)
    {{size.x / 2, -size.y / 2, -size.z / 2},  {1.0f, 0.0f, 0.0f},  {size.z, 0.0f  }},
    {{size.x / 2, size.y / 2, -size.z / 2},   {1.0f, 0.0f, 0.0f},  {size.z, size.y}},
    {{size.x / 2, size.y / 2, size.z / 2},    {1.0f, 0.0f, 0.0f},  {0.0f,   size.y}},
    {{size.x / 2, -size.y / 2, size.z / 2},   {1.0f, 0.0f, 0.0f},  {0.0f,   0.0f  }},

    // left face (x = -size.x/2)
    {{-size.x / 2, -size.y / 2, -size.z / 2}, {-1.0f, 0.0f, 0.0f}, {0.0f,   0.0f  }},
    {{-size.x / 2, -size.y / 2, size.z / 2},  {-1.0f, 0.0f, 0.0f}, {size.z, 0.0f  }},
    {{-size.x / 2, size.y / 2, size.z / 2},   {-1.0f, 0.0f, 0.0f}, {size.z, size.y}},
    {{-size.x / 2, size.y / 2, -size.z / 2},  {-1.0f, 0.0f, 0.0f}, {0.0f,   size.y}}
  };

  std::vector<uint> idx {
    0, 1, 2, 2, 3, 0,  // front
    4, 5, 6, 6, 7, 4,  // back
    8, 9, 10, 10, 11, 8,  // top
    12, 13, 14, 14, 15, 12,  // bottom
    16, 17, 18, 18, 19, 16,  // right
    20, 21, 22, 22, 23, 20  // left
  };

  return {std::move(verts), std::move(idx)};
}
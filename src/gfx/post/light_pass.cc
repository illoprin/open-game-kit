#include "light_pass.hpp"
#include "core./log.hpp"
#include "core/files.hpp"
#include "scene/collision.hpp"
#include <array>
#include <cstddef>
#include <cstring>
#include <glm/gtc/matrix_inverse.hpp>

namespace {

constexpr GLuint LightBlockBinding = 0;
constexpr size_t MaxLights         = 32;

struct alignas(16) LightingBlock {
  std::array<PointLight, MaxLights> PointLights{};
  std::array<SpotLight, MaxLights>  SpotLights{};
  glm::ivec4                        Counts{0};
};

static_assert(sizeof(PointLight) == 32);
static_assert(sizeof(SpotLight) == 64);
static_assert(
  offsetof(LightingBlock, SpotLights) == sizeof(PointLight) * MaxLights
);
static_assert(
  offsetof(LightingBlock, Counts)
  == sizeof(PointLight) * MaxLights + sizeof(SpotLight) * MaxLights
);

Frustum makeFrustum(const glm::mat4& viewProjection) {
  Frustum         frustum;
  const glm::vec4 row0(
    viewProjection[0][0],
    viewProjection[1][0],
    viewProjection[2][0],
    viewProjection[3][0]
  );
  const glm::vec4 row1(
    viewProjection[0][1],
    viewProjection[1][1],
    viewProjection[2][1],
    viewProjection[3][1]
  );
  const glm::vec4 row2(
    viewProjection[0][2],
    viewProjection[1][2],
    viewProjection[2][2],
    viewProjection[3][2]
  );
  const glm::vec4 row3(
    viewProjection[0][3],
    viewProjection[1][3],
    viewProjection[2][3],
    viewProjection[3][3]
  );
  frustum.planes = {
    row3 + row0,
    row3 - row0,
    row3 + row1,
    row3 - row1,
    row3 + row2,
    row3 - row2
  };
  for (glm::vec4& plane : frustum.planes) {
    plane /= glm::length(glm::vec3(plane));
  }
  return frustum;
}

}  // namespace

LightPass::LightPass() : lightBuffer(GL_UNIFORM_BUFFER, sizeof(LightingBlock)) {
  std::strcpy(name, "lighting");
  og_assert(
    Program::FastLoad(
      prog,
      ShaderPath("post/screen.vert"),
      ShaderPath("post/lights.frag")
    ),
    "failed to load lighting program"
  );
  prog.SetUniformBlockBinding("LightingBlock", LightBlockBinding);
}

void LightPass::RenderPass(const PostProcessingContext& ctx) noexcept {
  LightingBlock block;
  const Frustum frustum = makeFrustum(ctx.Proj * ctx.View);

  if (ctx.PointLights) {
    for (const PointLight& light : *ctx.PointLights) {
      if (block.Counts.x >= static_cast<int>(MaxLights)) break;
      if (
        frustum.ContainsSphere({glm::vec3(light.Position), light.Position.w})
      ) {
        block.PointLights[block.Counts.x++] = light;
      }
    }
  }

  if (ctx.SpotLights) {
    for (const SpotLight& light : *ctx.SpotLights) {
      if (block.Counts.y >= static_cast<int>(MaxLights)) break;
      if (
        frustum.ContainsSphere({glm::vec3(light.Position), light.Position.w})
      ) {
        block.SpotLights[block.Counts.y++] = light;
      }
    }
  }

  lightBuffer.Set(0, sizeof(block), &block);
  glBindBufferBase(GL_UNIFORM_BUFFER, LightBlockBinding, lightBuffer.ID());

  ctx.Destination->BindForDrawing(GL_COLOR_BUFFER_BIT);
  prog.Use();

  ctx.Color->Bind(0);
  ctx.Normal->Bind(1);
  ctx.Depth->Bind(2);
  prog.SetInt("u_color", 0);
  prog.SetInt("u_normal", 1);
  prog.SetInt("u_depth", 2);
  prog.SetMat4("u_inv_proj", glm::inverse(ctx.Proj));
  prog.SetMat4("u_inv_view", glm::inverse(ctx.View));

  ctx.BasicQuad->Draw();
}

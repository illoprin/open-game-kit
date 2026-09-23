#include "collision.hpp"
#include "glm/geometric.hpp"
#include "scene/geometry.hpp"
#include <algorithm>
#include <cmath>

bool Frustum::ContainsSphere(const Sphere& s) const {
  for (const auto& p : planes) {
    float dist = p.x * s.center.x + p.y * s.center.y + p.z * s.center.z + p.w;
    if (dist < -s.radius) return false;
  }
  return true;
}

Frustum Frustum::FromPerspective(
  float            fovY,
  float            aspect,
  float            zNear,
  float            zFar,
  const glm::mat4& viewMatrix
) {
  Frustum frustum;

  // Projection matrix calculation (assuming standard OpenGL projection)
  float tanHalfFovY = std::tan(fovY * 0.5f);

  float hNear = zNear * tanHalfFovY;
  float wNear = hNear * aspect;
  float hFar  = zFar * tanHalfFovY;
  float wFar  = hFar * aspect;

  // Center points of near and far planes in view space
  glm::vec3 nc(0.0f, 0.0f, -zNear);
  glm::vec3 fc(0.0f, 0.0f, -zFar);

  // Un-transform corners to world space using inverse view matrix (or build
  // directly via view projection)
  glm::mat4 invView = glm::inverse(viewMatrix);

  auto toWorld = [&](glm::vec3 p) {
    glm::vec4 wp = invView * glm::vec4(p, 1.0f);
    return glm::vec3(wp) / wp.w;
  };

  // Calculate planes from points (Normal points inward)
  // For a standard plane Ax + By + Cz + D = 0
  auto makePlane = [](glm::vec3 p0, glm::vec3 p1, glm::vec3 p2) {
    glm::vec3 normal = glm::normalize(glm::cross(p1 - p0, p2 - p0));
    float     d      = -glm::dot(normal, p0);
    return glm::vec4(normal, d);
  };

  // View space frustum corners
  glm::vec3 ntl = toWorld(nc + glm::vec3(-wNear, hNear, 0.0f));
  glm::vec3 ntr = toWorld(nc + glm::vec3(wNear, hNear, 0.0f));
  glm::vec3 nbl = toWorld(nc + glm::vec3(-wNear, -hNear, 0.0f));
  glm::vec3 nbr = toWorld(nc + glm::vec3(wNear, -hNear, 0.0f));

  glm::vec3 ftl = toWorld(fc + glm::vec3(-wFar, hFar, 0.0f));
  glm::vec3 ftr = toWorld(fc + glm::vec3(wFar, hFar, 0.0f));
  glm::vec3 fbl = toWorld(fc + glm::vec3(-wFar, -hFar, 0.0f));
  glm::vec3 fbr = toWorld(fc + glm::vec3(wFar, -hFar, 0.0f));

  frustum.planes[0] = makePlane(nbl, ntl, ftl);  // Left
  frustum.planes[1] = makePlane(ntr, nbr, ftr);  // Right
  frustum.planes[2] = makePlane(nbl, nbr, fbr);  // Bottom
  frustum.planes[3] = makePlane(ntl, ftl, ftr);  // Top
  frustum.planes[4] = makePlane(nbr, nbl, ntl);  // Near
  frustum.planes[5] = makePlane(ftl, fbl, fbr);  // Far

  return frustum;
}

namespace Collision {

bool Intersect(const AABB& a, const AABB& b) {
  return (a.min.x <= b.max.x && a.max.x >= b.min.x)
         && (a.min.y <= b.max.y && a.max.y >= b.min.y)
         && (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

bool Intersect(const AABB& aabb, const Sphere& sphere) {
  float sqDist = 0.f;
  auto  check  = [&](float v, float minVal, float maxVal) {
    if (v < minVal)
      sqDist += (minVal - v) * (minVal - v);
    else if (v > maxVal)
      sqDist += (v - maxVal) * (v - maxVal);
  };
  check(sphere.center.x, aabb.min.x, aabb.max.x);
  check(sphere.center.y, aabb.min.y, aabb.max.y);
  check(sphere.center.z, aabb.min.z, aabb.max.z);
  return sqDist <= sphere.radius * sphere.radius;
}

bool Intersect(const AABB& aabb, const Line& line) {
  float     tmin = 0.0f;
  float     tmax = 1.0f;
  glm::vec3 dir  = line.end - line.start;

  auto checkAxis =
    [&](float startCoord, float dirCoord, float minCoord, float maxCoord) {
      if (std::abs(dirCoord) < 1e-8f) {
        return startCoord >= minCoord && startCoord <= maxCoord;
      }
      float ood = 1.0f / dirCoord;
      float t1  = (minCoord - startCoord) * ood;
      float t2  = (maxCoord - startCoord) * ood;
      if (t1 > t2) std::swap(t1, t2);
      tmin = std::max(tmin, t1);
      tmax = std::min(tmax, t2);
      return tmin <= tmax;
    };

  return checkAxis(line.start.x, dir.x, aabb.min.x, aabb.max.x)
         && checkAxis(line.start.y, dir.y, aabb.min.y, aabb.max.y)
         && checkAxis(line.start.z, dir.z, aabb.min.z, aabb.max.z);
}

bool Intersect(const AABB& aabb, const Capsule& capsule) {
  AABB expanded{
    {aabb.min.x - capsule.radius,
     aabb.min.y - capsule.radius,
     aabb.min.z - capsule.radius},
    {aabb.max.x + capsule.radius,
     aabb.max.y + capsule.radius,
     aabb.max.z + capsule.radius}
  };
  return Intersect(expanded, Line{capsule.a, capsule.b});
}


}  // namespace Collision

#version 330 core

out vec4 out_frag_color;

in vec2 texcoord;

uniform sampler2D u_color;
uniform sampler2D u_normal;
uniform sampler2D u_depth;
uniform mat4 u_inv_proj;
uniform mat4 u_inv_view;

struct PointLight {
  vec4 Position; // xyz: pos, w: radius
  vec4 Color; // rgb: color, a: intensity
};

struct SpotLight {
  vec4 Position; // xyz: pos, w: radius
  vec4 Color; // rgb: color, a: intensity
  vec4 Dir; // xyz: direction vector, w: padding
  vec4 Data; // xy: cos outer, inner; z: smoothness; w: padding 
};

layout (std140) uniform LightingBlock {
  PointLight point_lights[32];
  SpotLight spot_lights[32];
  ivec4 u_light_counts;
};

vec3 ReconstructPositionWorldSpace(vec2 uv, float depth) {
  // NDC: x, y в [-1,1], z в [-1,1] (OpenGL)
  vec4 ndc = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
  vec4 world_pos_h = (u_inv_view * u_inv_proj) * ndc;
  return world_pos_h.xyz / world_pos_h.w;
}

void main() {
  float depth = texture(u_depth, texcoord).r;

  // skip background/skybox
  if (depth == 1.0) {
    out_frag_color = texture(u_color, texcoord);
    return;
  }

  // reconstruct world position from depth
  vec3 world_pos = ReconstructPositionWorldSpace(texcoord, depth);
  // reconstruct world space normal
  vec3 normal = normalize(mat3(u_inv_view) * texture(u_normal, texcoord).rgb);
  vec3 albedo = texture(u_color, texcoord).rgb;

  vec3 total_lighting = vec3(0);

  // process point lights
  for (int i = 0; i < u_light_counts.x; ++i) {
    vec3 light_dir = point_lights[i].Position.xyz - world_pos;
    float dist = length(light_dir);
    float radius = point_lights[i].Position.w;

    if (dist < radius) {
      float attenuation = pow(clamp(1.0 - dist / radius, 0.0, 1.0), 2.0);
      float diff = max(dot(normal, normalize(light_dir)), 0.0);
      total_lighting += point_lights[i].Color.rgb * point_lights[i].Color.a * diff * attenuation;
    }
  }

  // process spot lights
  for (int i = 0; i < u_light_counts.y; ++i) {
    vec3 light_vec = spot_lights[i].Position.xyz - world_pos;
    float dist = length(light_vec);
    float radius = spot_lights[i].Position.w;

    if (dist < radius) {
      vec3 L = normalize(light_vec);

      // normalize spot directon
      vec3 D = normalize(spot_lights[i].Dir.xyz);

      // L - vec from fragment to -> source
      float cos_theta = dot(L, -D);

      float cos_outer = spot_lights[i].Data.x;
      float cos_inner = spot_lights[i].Data.y;

      // smooth
      float spot_intensity = smoothstep(cos_outer, cos_inner, cos_theta);

      if (spot_intensity > 0.0) {
        float attenuation = pow(clamp(1.0 - dist / radius, 0.0, 1.0), 2.0);
        float diff = max(dot(normal, L), 0.0);

        vec3 light_res = spot_lights[i].Color.rgb * spot_lights[i].Color.a;
        total_lighting += light_res * diff * attenuation * spot_intensity;
      }
    }
  }

  out_frag_color = vec4(albedo + albedo * total_lighting, 1.0);
}
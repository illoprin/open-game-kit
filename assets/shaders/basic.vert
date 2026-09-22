#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texcoord;

uniform mat4 u_projection = mat4(1.0);
uniform mat4 u_view = mat4(1.0);
uniform mat4 u_model = mat4(1.0);

out vec2 texcoord;
out vec3 world_pos;
out vec3 normal;

void main() {
  texcoord = in_texcoord;
  vec4 worldPos = u_model * vec4(in_position, 1.0);
  world_pos = worldPos.xyz;
  normal = normalize(mat3(u_model) * in_normal);
  gl_Position = u_projection * u_view * u_model * vec4(in_position, 1.0);
}
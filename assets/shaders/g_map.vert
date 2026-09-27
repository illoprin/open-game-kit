#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texcoord;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

out vec3 v_world_position;
out vec3 v_world_normal;
out vec2 v_texcoord;

void main() {
  vec4 world_position = u_model * vec4(in_position, 1.0);

  v_world_position = world_position.xyz;

  mat3 normal_matrix = mat3(transpose(inverse(u_model)));
  v_world_normal = normalize(normal_matrix * in_normal);

  v_texcoord = in_texcoord;

  gl_Position = u_projection *
    u_view *
    world_position;
}
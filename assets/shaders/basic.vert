#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texcoord;

out vec2 texcoord;

uniform mat4 u_projection = mat4(1.0);
uniform mat4 u_view = mat4(1.0);
uniform mat4 u_model = mat4(1.0);

uniform float u_uv_scaling = 1.0;

void main() {
  texcoord = in_texcoord * u_uv_scaling;
  gl_Position = u_projection * u_view * u_model * vec4(in_position, 1.0);
}
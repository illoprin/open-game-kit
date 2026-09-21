#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texcoord;

out vec2 texcoord;

uniform mat4 u_pv = mat4(1.0);

void main() {
  texcoord = in_texcoord;
  gl_Position = u_pv * vec4(in_position, 1.0);
}
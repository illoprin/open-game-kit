#version 330 core

in vec2 texcoord;

uniform sampler2D u_diffuse;

out vec4 out_fragcolor;

void main() {
  vec4 out_color = texture(u_diffuse, texcoord);
  if (out_color.a < 0.1) discard;
  out_fragcolor = out_color;
}
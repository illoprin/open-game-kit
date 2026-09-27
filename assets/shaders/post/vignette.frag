#version 330 core

in vec2 texcoord;

uniform sampler2D u_color;
uniform float u_radius;
uniform float u_softness;

out vec4 out_frag_color;

void main() {
  vec4 result = texture(u_color, texcoord);
  vec3 color = result.rgb;

  // apply vignette
  float amount = smoothstep(u_radius, u_radius - u_softness, length(texcoord - vec2(.5)));
  color *= amount;

  out_frag_color = vec4(color, result.a);
}
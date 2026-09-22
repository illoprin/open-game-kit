#version 330 core

in vec2 texcoord;
in vec3 world_pos;
in vec3 normal;

uniform sampler2D u_diffuse;
uniform bool u_use_triplanar = false;
uniform float u_uv_scaling = 1.0;

out vec4 out_fragcolor;

vec3 TriplanarUV(vec3 pos, vec3 normal, float scale) {
  vec3 blend = abs(normal);
  blend /= (blend.x + blend.y + blend.z);

  vec3 uvX = texture(u_diffuse, pos.yz * scale).rgb;
  vec3 uvY = texture(u_diffuse, pos.xz * scale).rgb;
  vec3 uvZ = texture(u_diffuse, pos.xy * scale).rgb;

  return uvX * blend.x + uvY * blend.y + uvZ * blend.z;
}

void main() {
  if (u_use_triplanar) {
    vec3 out_color = TriplanarUV(world_pos, normal, u_uv_scaling);
    out_fragcolor = vec4(out_color, 1.0);
  } else {
    vec4 out_color = texture(u_diffuse, texcoord);
    if (out_color.a < 0.1) discard;
    out_fragcolor = out_color;
  }
}
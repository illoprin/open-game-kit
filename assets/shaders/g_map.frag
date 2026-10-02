#version 330 core

in vec3 v_world_position;
in vec3 v_world_normal;
in vec2 v_texcoord;

uniform sampler2D u_diffuse;
uniform bool u_use_triplanar = false;
uniform bool u_use_diffuse = true;
uniform float u_uv_scaling = 1.0;
uniform vec3 u_tint = vec3(1.0);

out layout(location = 0) vec4 out_fragcolor;
out layout(location = 1) vec3 out_normal;

// sunlight
uniform vec3 u_sun_direction;
uniform vec3 u_sun_color;
uniform float u_sun_intensity;

// ambient
uniform vec3 u_ambient_color;
uniform float u_ambient_intensity;

// matrices
uniform mat4 u_view;

vec3 TriplanarUV(vec3 pos, vec3 normal, float scale) {
  vec3 blend = abs(normal);
  blend /= (blend.x + blend.y + blend.z);

  vec3 uvX = texture(u_diffuse, pos.yz * scale).rgb;
  vec3 uvY = texture(u_diffuse, pos.xz * scale).rgb;
  vec3 uvZ = texture(u_diffuse, pos.xy * scale).rgb;

  return uvX * blend.x + uvY * blend.y + uvZ * blend.z;
}

vec3 lighting() {

  vec3 normal = normalize(v_world_normal);

    // ambient light
  vec3 ambient = u_ambient_color *
    u_ambient_intensity;

    // sunlight
  vec3 sun_direction = normalize(-u_sun_direction);

  float diffuse = max(dot(normal, sun_direction), 0.0);

  vec3 sunlight = u_sun_color *
    u_sun_intensity *
    diffuse;

  return ambient + sunlight;
}

void main() {
  out_normal = normalize(mat3(u_view) * v_world_normal);
  
  // calculate lighting
  vec3 lighting = lighting();

  vec3 base_color = u_tint;

  if (u_use_diffuse) {
    vec4 tex;
    if (u_use_triplanar) {
      tex = vec4(TriplanarUV(v_world_position, v_world_normal, u_uv_scaling), 1.0);
    } else {
      tex = texture(u_diffuse, v_texcoord);
      if (tex.a < 0.1)
        discard;
    }
    base_color *= tex.rgb;
  }

  base_color *= lighting;
  out_fragcolor = vec4(base_color, 1.0);
}

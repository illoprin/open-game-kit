#version 330 core

out vec4 frag_color;

uniform vec4 u_color = vec4(0.0, 1.0, 0.0, 1.0); // Default green wireframe

void main() {
  frag_color = u_color;
}
#version 460 core
layout(location = 0) in vec3 in_postion;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

out vec2 uv;

void main() {
  vec4 position = vec4(in_postion.xyz, 1);
  gl_Position = projection * view * model * position;
  uv = in_uv;
}
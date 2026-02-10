#version 460 core
in vec2 uv;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D block_textures;

void main() {
  FragColor = texture(block_textures, vec2(uv.x / 4.f, uv.y / 4.f));
}
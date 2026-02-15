#version 460 core
in vec2 uv;
in vec3 fragement_position;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D block_textures;
uniform vec3 view_position;

void main() {
  // 物体颜色
  vec4 object_color = texture(block_textures, vec2(uv.x / 4.f, uv.y / 4.f));
  vec3 fog_color = vec3(1.f);
  float density = 0.01f;

  // 计算距离（世界空间欧几里得距离）
  float dist = length(fragement_position - view_position);

  // 线性雾因子
  float fog_factor = exp(-pow(dist * density, 2.0));
  fog_factor = clamp(fog_factor, 0.0, 1.0);
  FragColor = vec4(mix(fog_color, object_color.rgb, fog_factor), 1.);
}
#version 460 core
in vec2 uv;
in vec3 normal;
in vec3 fragement_position;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D block_textures;
uniform vec3 view_position;
uniform vec3 light_direction;

void main() {
  float diffuse = max(dot(normal, light_direction), 0.f);
  float ambient = 0.3f;

  vec4 object_color = texture(block_textures, vec2(uv.x / 4.f, uv.y / 4.f));
  object_color = (diffuse + ambient) * object_color;
  vec3 fog_color = vec3(1.f);
  float density = 0.01f;
  float dist = length(fragement_position - view_position);
  float fog_factor = exp(-pow(dist * density, 2.f));
  fog_factor = clamp(fog_factor, 0.f, 1.f);
  FragColor = vec4(mix(fog_color, object_color.rgb, fog_factor), 1.f);
}
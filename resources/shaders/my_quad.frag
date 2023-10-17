#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 color;

layout (binding = 0) uniform sampler2D colorTex;

layout (location = 0 ) in VS_OUT
{
  vec2 texCoord;
} surf;

float coef_by_var(float var) {
  return -1 / (2 * var * var);
}

void main()
{
  const ivec2 conv_size      = ivec2(4, 4);
  const float variance_space = 0.5;
  const float variance_color = 0.1;

  const float coef_space = coef_by_var(variance_space);
  const float coef_color = coef_by_var(variance_color);

  float sum_weight = 0;
  vec4  sum_color  = vec4(0);

  vec2 center_coord = surf.texCoord;
  vec4 center_color = textureLod(colorTex, center_coord, 0);

  for (float dx = -conv_size.x / 2; dx <= +conv_size.x / 2; dx = dx + 1) {
    for (float dy = -conv_size.y / 2; dy <= +conv_size.y / 2; dy = dy + 1) {
      vec2 shift = vec2(dx, dy);
      vec2 cur_coord = center_coord + shift / textureSize(colorTex, 0);
      vec4 cur_color = textureLod(colorTex, cur_coord, 0);

      float dist_space = length(cur_coord - center_coord);
      float dist_color = length(cur_color - center_color);

      float weight_space = coef_space * dist_space * dist_space;
      float weight_color = coef_color * dist_color * dist_color;
      
      float cur_weight = exp(weight_space + weight_color);

      sum_color  += cur_color * cur_weight;
      sum_weight += cur_weight;
    }
  }

  color = sum_color / sum_weight;
  // color = textureLod(colorTex, center_coord, 0);
}

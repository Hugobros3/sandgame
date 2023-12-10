#version 130

#define outputColor gl_FragData[0]

uniform isampler2D the_texture;
uniform int width;
uniform int height;

vec4 voxel_color(int t) {
  if (t == 0)
    return vec4(0.0);
  if (t == 1)
    return vec4(0.5, 0.5, 0.5, 1.0);
  if (t == 2)
    return vec4(0.8, 0.8, 0.0, 1.0);
  return vec4(1.0, 0.0, 1.0, 1.0);
}

void main() {
    outputColor = voxel_color(texture(the_texture, gl_FragCoord.xy / vec2(width, height)).r);
}

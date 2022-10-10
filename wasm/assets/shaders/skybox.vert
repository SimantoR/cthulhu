precision mediump float;

attribute vec3 v_position;

varying vec3 tex_coords;

uniform mat4 projection;
uniform mat4 view;

void main() {
  tex_coords = v_position;
  gl_Position = projection * view * vec4(v_position, 1.0);
}

precision mediump float;

attribute vec3 v_position;
attribute vec3 v_normal;
attribute vec2 v_uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

varying vec3 normal;
varying vec2 uv;

void main() {
    normal = v_normal;
    uv = v_uv;
    gl_Position = proj * view * model * vec4(v_position, 1.0);
}

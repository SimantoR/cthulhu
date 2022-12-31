precision mediump float;

attribute vec3 v_position;
attribute vec3 v_normal;
attribute vec2 v_uvcoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

varying vec3 FragPos;
varying vec3 Normal;
varying vec2 UVCoords;
varying vec4 FragPosLightSpace;

mat2 transpose(mat2 m) {
  return mat2(m[0][0], m[1][0],
              m[0][1], m[1][1]);
}

mat3 transpose(mat3 m) {
  return mat3(m[0][0], m[1][0], m[2][0],
              m[0][1], m[1][1], m[2][1],
              m[0][2], m[1][2], m[2][2]);
}

mat4 transpose(mat4 m) {
  return mat4(m[0][0], m[1][0], m[2][0], m[3][0],
              m[0][1], m[1][1], m[2][1], m[3][1],
              m[0][2], m[1][2], m[2][2], m[3][2],
              m[0][3], m[1][3], m[2][3], m[3][3]);
}

void main() {
    FragPos = vec3(model * vec4(v_position, 1.0));
    Normal = vec3(transpose(inverse(mat3(model))) * v_normal);
    UVCoords = v_uvcoords;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

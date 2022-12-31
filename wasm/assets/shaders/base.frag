/* ---------------------------- Base Shader Code ---------------------------- */
precision mediump float;
 
struct DirLight {
    vec3 direction;
    vec3 color;
    float strength;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float strength;
    float radius;
};

varying vec3 normal;
varying vec2 uv;

uniform g_dirLightCount;
uniform DirLight g_dirLights;

uniform g_pointLightCount;
uniform PointLight g_pointLights;

/* ------------------------ User Defined Shader Code ------------------------ */
uniform customVariable;

void main() {

}

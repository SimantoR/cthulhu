precision mediump float;
 
struct DirLight {
    vec3 direction;
    vec3 color;
    float strength;
};

varying vec3 normal;
varying vec2 uv;

uniform DirLight light;

// uniform vec3 color;

uniform sampler2D diffuseTex;

void applyLighting()
{
    vec3 color = vec3(texture2D(diffuseTex, uv));
    // vec3 color = vec3(1.0, 1.0, 1.0);

    // Get reverse direction of light
    vec3 lightDir = normalize(-light.direction);

    // Dot product between normal and light's direction will produce how intense light needs to be.
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.strength * diff * color;

    // Mix the light's color for a sun-like feel
    diffuse = mix(diffuse, light.color, 0.05);

    gl_FragColor = vec4(diffuse, 1.0);
}

void main() {
    // TODO: Apply model matrix to light direction to get correct rotation of mesh
    
    // gl_FragColor = texture2D(diffuseTex, uv);
    // gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    applyLighting();
}

#version 330 core

// STAR SPHERE FRAGMENT SHADER

// Interpolated values from the vertex shaders
in vec3 Normal_cameraspace;
in vec2 uv;

// Ouput data
layout(location = 0) out vec3 color;
layout(location = 1) out vec3 emission;


float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    float b = .0;
    for (int i = 0; i < 500; i++) {
        vec2 p = vec2(rand(vec2(57 + i, i - 22)), rand(vec2(5 - i, 10 + i)));

        if (length(uv - p) < 0.0005 * rand(vec2(221 + i, i - 422))) {
            b = 1.;
        }

    }

    b = clamp(b, 0, 1);

    color = vec3(b, b, b);
    emission = vec3(0);
}
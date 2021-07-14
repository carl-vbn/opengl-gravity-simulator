#version 330 core

// UI FRAGMENT SHADER

in vec4 interpolatedVertexColor;
in vec2 uv;
layout(location = 0) out vec4 color;
layout(location = 1) out vec3 emission;

uniform bool UseTexture;
uniform sampler2D textureData;

void main() {
    if (UseTexture) {
        color = texture(textureData, uv) * interpolatedVertexColor;
    }
    else {
        color = interpolatedVertexColor;
    }

    emission = vec3(0);
}
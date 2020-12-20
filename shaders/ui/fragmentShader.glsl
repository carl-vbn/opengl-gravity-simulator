#version 330 core

// UI FRAGMENT SHADER

in vec4 interpolatedVertexColor;
in vec2 uv;
out vec4 color;

uniform bool UseTexture;
uniform sampler2D textureData;

void main() {
    if (UseTexture) {
        color = texture(textureData, uv) * interpolatedVertexColor;
    }
    else {
        color = interpolatedVertexColor;
    }
}
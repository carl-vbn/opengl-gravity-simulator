#version 330 core

// POST PROCESSING SHADER THAT AFFECTS THE ENTIRE SCREEN ONCE EVERYTHING HAS BEEN RENDERED

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D ScreenTexture;
uniform sampler2D EmissionTexture;

// Gaussian blur kernel
float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 tex_offset = 1.0 / textureSize(EmissionTexture, 0); // gets size of single texel
    vec3 bloomColor = texture(EmissionTexture, TexCoords).rgb * weight[0]; // current fragment's contribution
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            //float offsetY = (TexCoords + vec2(tex_offset.x * i, tex_offset.y * -j)).y;
            if ((TexCoords + vec2(tex_offset.x * i, tex_offset.y * -j)).y < 0.0 || (TexCoords + vec2(tex_offset.x * i, tex_offset.y * j)).y > 1.0) continue;

            // This can be optimized by doing a horizontal and vertical pass separately
            bloomColor += texture(EmissionTexture, TexCoords + vec2(tex_offset.x * i, tex_offset.y * j)).rgb * weight[i];
            bloomColor += texture(EmissionTexture, TexCoords + vec2(tex_offset.x * -i, tex_offset.y * j)).rgb * weight[i];
            bloomColor += texture(EmissionTexture, TexCoords + vec2(tex_offset.x * i, tex_offset.y * -j)).rgb * weight[i];
            bloomColor += texture(EmissionTexture, TexCoords + vec2(tex_offset.x * -i, tex_offset.y * -j)).rgb * weight[i];
        }
    }

    const float gamma = 2.2;
    const float exposure = 1.0;
    vec3 hdrColor = texture(ScreenTexture, TexCoords).rgb;
    hdrColor += 0.5*bloomColor; // additive blending
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
}
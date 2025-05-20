#version 330 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D bloomTexture;
uniform sampler2D screenTexture;

void main() 
{
    vec3 fragment = texture(screenTexture, texCoords).rgb;
    vec3 bloom = texture(bloomTexture, texCoords).rgb;
    // vec4(1) - 
    //FragColor = vec4(vec3(fragment + bloom), 1);

    // 
    FragColor = vec4(vec3(bloom), 1);
}
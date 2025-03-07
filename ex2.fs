/******************************************************************************
 * This is a really simple fragment shader that simply sets the output fragment
 * color to yellow.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core

in vec3 shaderColor;
in vec2 shaderTexCoord;
uniform sampler2D shaderTextureA;
uniform sampler2D shaderTextureB;
out vec4 fragmentColor;
uniform float t;
vec2 transform = vec2(sin(t), sin(t));
// vec2 transform = vec2(1.0f, 1.0f);

vec4 colorA = texture(shaderTextureA, shaderTexCoord);
vec4 colorB = texture(shaderTextureB, shaderTexCoord * transform);


void main()
{
    // fragmentColor = vec4(shaderColor, sin(t)) * texture(shaderTextureA, shaderTexCoord) * texture(shaderTextureB, shaderTexCoord);
    fragmentColor = mix(colorA, colorB, 0.5f);
}

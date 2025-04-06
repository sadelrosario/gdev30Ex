/******************************************************************************
 * This is a really simple fragment shader that simply sets the output fragment
 * color to yellow.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core

in vec3 shaderColor;
in vec2 shaderTexCoord;
in vec2 shaderSide;
uniform sampler2D shaderTextureA;
uniform sampler2D shaderTextureB;
out vec4 fragmentColor;

vec4 colorA = texture(shaderTextureA, shaderTexCoord);
vec4 colorB = texture(shaderTextureB, shaderTexCoord);

void main()
{
    vec4 shade = vec4(shaderColor.x - 0.19f, shaderColor.y - 0.33f, shaderColor.z - 0.34f, 1.0f);
    vec4 darkerShade = vec4(shaderColor.x - 0.39f, shaderColor.y - 0.51f, shaderColor.z - 0.52f, 1.0f);

    if(shaderSide.y == 0) {
        fragmentColor = colorA;
    }
    if(shaderSide.y == 1) {
        fragmentColor = colorB;
    }
    if(shaderSide.x == 2) {
        fragmentColor = colorB * shade;
    }
    if(shaderSide.x == 3) {
        fragmentColor = colorB * darkerShade;
    }
}

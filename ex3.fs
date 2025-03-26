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
out vec4 fragmentColor;
// vec2 transform = vec2(sin(t), sin(t));
// vec2 transform = vec2(1.0f, 1.0f);

vec4 colorA = texture(shaderTextureA, shaderTexCoord);
// vec4 colorB = texture(shaderTextureB, shaderTexCoord * transform);


void main()
{
    vec4 shade = vec4(shaderColor.x - 0.19f, shaderColor.y - 0.33f, shaderColor.z - 0.34f, 1.0f);
    vec4 darkerShade = vec4(shaderColor.x - 0.39f, shaderColor.y - 0.51f, shaderColor.z - 0.52f, 1.0f);

    fragmentColor = colorA;
    if(shaderSide.x == 2) {
        fragmentColor = colorA * shade;
    }
    if(shaderSide.x == 3) {
        fragmentColor = colorA * darkerShade;
    }
}

/******************************************************************************
 * This is a really simple fragment shader that simply sets the output fragment
 * color to yellow.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core

in vec3 shaderColor;
in vec2 shaderTexCoord;
in vec2 shaderAnim;
uniform sampler2D shaderTextureA;
uniform sampler2D shaderTextureB;
out vec4 fragmentColor;
uniform float t;
uniform float d;
vec2 transform = vec2(sin(t), sin(t));
// vec2 transform = vec2(1.0f, 1.0f);

vec4 colorA = texture(shaderTextureA, shaderTexCoord);
vec4 colorB = texture(shaderTextureB, shaderTexCoord * transform);


void main()
{
    // fragmentColor = vec4(shaderColor, sin(t)) * texture(shaderTextureA, shaderTexCoord) * texture(shaderTextureB, shaderTexCoord);
    // fragmentColor = mix(colorA, colorB, 0.5f);
    // mix(vec3(shaderColor), vec3(0.82f, 0.23f, 0.23f), (abs(sin(t + 1) * 0.5f)))

    vec4 colorA = texture(shaderTextureA, shaderTexCoord);
    vec4 red = vec4(shaderColor.x - 0.18f, shaderColor.y - 0.77f, shaderColor.z - 0.77f, 1.0f);

    if (shaderAnim.y > 0 && d < 0) {
        fragmentColor = mix(vec4(colorA), red+colorA, abs(clamp(sin(t), 0, 1)));
    }
    else {
        fragmentColor = colorA;
    }
    


}

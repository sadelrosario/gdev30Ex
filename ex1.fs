/******************************************************************************
 * This is a really simple fragment shader that simply sets the output fragment
 * color to yellow.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core

uniform float seconds;
uniform float glow;
in float eyeAnimation;
in vec3 shaderColor;
out vec4 fragmentColor;

void main()
{
    
    // fragmentColor = vec4(shaderColor, 1.0f);
    if (eyeAnimation == 4) {
        fragmentColor = vec4(shaderColor, 1.0f); // do not change opacity of eye to give glow effect 
    }
    else {
        fragmentColor = vec4(mix(shaderColor, shaderColor * glow,  abs(cos(seconds * 0.7)+1/2)), 1.0f);
        // dim everything else
    }
}

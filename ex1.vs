/******************************************************************************
 * This is a really simple vertex shader that simply sets the output vertex's
 * position to be the same as the input.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core

layout (location = 0) in vec3 vertexPosition; // we are adding a new attribute
layout (location = 1) in vec3 vertexColor;
layout (location = 2) in vec2 vertexAnim;
uniform vec4 browChange;
uniform float glow;
uniform float animate;
uniform float color;
uniform float seconds;
uniform float frown;
uniform vec3 colorChange;
out vec3 shaderColor;

void main()
{
    gl_Position = vec4(vertexPosition.x, vertexPosition.y + (clamp(2 * cos(seconds * 0.7f), 0.0f, 3.0f)), vertexPosition.z, 1.0f); 

    // check animatable elements (per vertex)
    // Frown Animation
    if (vertexAnim.x == 1 && vertexAnim.y == 1 && animate == 2) { // check if left brow bottom
        gl_Position = vec4(vertexPosition.x + (browChange.x), vertexPosition.y - (browChange.y), vertexPosition.z, 1.0f);   
    }
    if (vertexAnim.x == 1 && vertexAnim.y == 2 && animate == 2) { // check if left brow top
        gl_Position = vec4(vertexPosition.x + (browChange.z), vertexPosition.y - (browChange.w), vertexPosition.z, 1.0f);
    } 
    if (vertexAnim.x == 2 && vertexAnim.y == 1 && animate == 2) { // check if right brow bottom
        gl_Position = vec4(vertexPosition.x + (browChange.x), vertexPosition.y - (browChange.y), vertexPosition.z, 1.0f);
    }
    if (vertexAnim.x == 2 && vertexAnim.y == 2 && animate == 2) { // check if right brow top
        gl_Position = vec4(vertexPosition.x + (browChange.z), vertexPosition.y - (browChange.w), vertexPosition.z, 1.0f);
    }
    if (vertexAnim.x == 3 && vertexAnim.y == 1 && animate == 2) { // check if left corner lip
        gl_Position = vec4(vertexPosition.x, vertexPosition.y - (0.010f), vertexPosition.z, 1.0f);
    }
    if (vertexAnim.x == 3 && vertexAnim.y == 2 && animate == 2) { // check if right corner lip
        gl_Position = vec4(vertexPosition.x, vertexPosition.y + (0.010f), vertexPosition.z, 1.0f);
    }
    if (vertexAnim.x == 3 && vertexAnim.y == 3 && animate == 2) { // check if middle lip
        gl_Position = vec4(vertexPosition.x, vertexPosition.y - (0.010f), vertexPosition.z, 1.0f);
    }
    
    // check glowable elements
    if (vertexAnim.x == 4 && vertexAnim.y == 0 && color == 1) { 
        shaderColor = colorChange;
    } 
    else {
        shaderColor = vertexColor * glow;
    }
}

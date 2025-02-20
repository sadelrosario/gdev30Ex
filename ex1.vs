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
uniform float up;
uniform float animate;
uniform float seconds;
out vec3 shaderColor;

void main()
{
    // gl_Position = vec4(vertexPosition.x, vertexPosition.y + 3, vertexPosition.z, 1.0f); 
    gl_Position = vec4(vertexPosition.x, vertexPosition.y + (up * clamp(3*cos(seconds), 0.0f, 2.0f)), vertexPosition.z, 1.0f); 


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
    // if (vertexAnim.x == 4) { 
    //   shaderColor = vec3(vertexColor.xyz * glow);
    // }

    shaderColor = vertexColor;
}

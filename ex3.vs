/******************************************************************************
 * This is a really simple vertex shader that simply sets the output vertex's
 * position to be the same as the input.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core

layout (location = 0) in vec3 vertexPosition; // we are adding a new attribute
layout (location = 1) in vec3 vertexColor;
layout (location = 2) in vec2 vertexTexCoord;
out vec3 shaderColor;
out vec2 shaderTexCoord;

uniform mat4 matrix;

void main()
{
    gl_Position = matrix * vec4(vertexPosition.x, vertexPosition.y, vertexPosition.z, 1.0f); // 4th attribute is a homogenous coordinate (the w) 
    shaderColor = vertexColor;
    shaderTexCoord = vertexTexCoord;
}

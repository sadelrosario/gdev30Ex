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
layout (location = 3) in vec2 sideIndicator;
layout (location = 4) in vec3 vertexNormal;
out vec3 shaderColor;
out vec2 shaderTexCoord;
out vec2 shaderSide;

out vec3 worldSpacePosition;
out vec3 worldSpaceNormal;
out vec3 objectColor;
// matrices
uniform mat4 projectionViewMatrix;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;

void main()
{
    worldSpacePosition = (modelMatrix * vec4(vertexPosition, 1.0f)).xyz;
    worldSpaceNormal = (normalMatrix * vec4(vertexNormal, 1.0f)).xyz;
    gl_Position = projectionViewMatrix * vec4(worldSpacePosition, 1.0f); // 4th attribute is a homogenous coordinate (the w) 
    
    // pass to fragment
    shaderColor = vertexColor;
    shaderTexCoord = vertexTexCoord;
    shaderSide = sideIndicator;
    objectColor = shaderColor;
}

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

// layout (location = 0) in vec3 vertex2_Position; 
// layout (location = 1) in vec3 vertex2_Color;
// layout (location = 2) in vec2 vertex2_TexCoord;
// layout (location = 3) in vec3 vertex2_Side;
// layout (location = 4) in vec3 vertex2_Normal;

out vec3 shaderColor;
out vec2 shaderTexCoord;
out vec2 shaderSide;

// out vec3 shader2_Color;
// out vec2 shader2_TexCoord;
// out vec2 shader2_Side;

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

    // gl_Position = vec4(vertex2_Position, 1.0f);
    // shader2_Color = vertex2_Color;
}

/******************************************************************************
 * This is a really simple vertex shader that simply sets the output vertex's
 * position to be the same as the input.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core
// these are inputs
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexColor; // adding new location of color
// we output the input
layout (location = 2) in vec2 vertexTexCoord;
layout (location = 3) in vec2 vertexSetter;
out vec3 shaderColor;
out vec2 shaderTexCoord;
out vec2 shaderSetter;
// out vec3 shaderColor; // this output will be put into the fragment shader

void main()
{
    gl_Position = vec4(vertexPosition.x, vertexPosition.y, vertexPosition.z, 1.0f);
    // position data in openGL is a 4 dimensional value
    // 1.0f is important to set = a homogenous coordinate (w)
    // ^ used for perspective divide
    shaderColor = vertexColor;
    shaderTexCoord = vertexTexCoord;
    shaderSetter = vertexSetter;
    // vertexColor is input from the cpp 
    // shaderColor is the output that contains the value of the input 
    // which will be inputted to the fragment shader
}

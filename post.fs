#version 330 core

in vec4 fragmentColor;
in vec4 bloomColor;
out vec4 FragColor;

void main()
{
    FragColor = fragmentColor + bloomColor;
}
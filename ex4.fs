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

in vec3 worldSpacePosition;
in vec3 worldSpaceNormal;
in vec3 objectColor;

// textures
uniform sampler2D shaderTextureA;
uniform sampler2D shaderTextureB;

// camera
uniform vec3 cameraPos;

out vec4 fragmentColor;

vec4 colorA = texture(shaderTextureA, shaderTexCoord);
vec4 colorB = texture(shaderTextureB, shaderTexCoord);

vec3 lightPosition = vec3(9.0f, 9.0f, 9.0f); // light source position
vec3 lightColor = vec3(1.0f, 0.95f, 0.76f); // light source color

void main()
{
    vec4 shade = vec4(shaderColor.x - 0.19f, shaderColor.y - 0.33f, shaderColor.z - 0.34f, 1.0f); // solid coloring shade, not lighting related
    vec4 darkerShade = vec4(shaderColor.x - 0.39f, shaderColor.y - 0.51f, shaderColor.z - 0.52f, 1.0f); // solid coloring shade, not lighting related

    // diffuse lighting 
    vec3 lightVector = normalize(lightPosition - worldSpacePosition); // l
    vec3 reNormalizeWorldNormal = normalize(worldSpaceNormal); // n
    float diffuseColor = max(dot(reNormalizeWorldNormal, lightVector), 0); // Cd
    // ambient lighting
    float ambientColor = 0.7f;
    // specular lighting
    // float reflection = lightVector - 2 * dot(reNormalizeWorldNormal, lightVector) * reNormalizeWorldNormal;
    vec3 reflectionVector = reflect(-lightVector, reNormalizeWorldNormal);
    float shininess = 32;
    float specularLight = max(pow(dot(reflectionVector, cameraPos), shininess), 0);
    fragmentColor = vec4(((diffuseColor + ambientColor + specularLight) * lightColor), 1.0f);

    

    // adding textures and solid color shading
    if(shaderSide.y == 0) {
        fragmentColor *= colorA;
    }
    if(shaderSide.y == 1) {
        fragmentColor *= colorB;
    }
    if(shaderSide.x == 2) {
        fragmentColor *= colorB;
    }
    if(shaderSide.x == 3) {
        fragmentColor *= colorB; // removed multiply to darkerShade
    }
}


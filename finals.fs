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

// in vec3 shader2_Color;
// in vec2 shader2_TexCoord;
// in vec2 shader2_Side;

in vec3 worldSpacePosition;
in vec3 worldSpaceNormal;
in vec3 objectColor;

// textures
uniform sampler2D shaderTextureA;
uniform sampler2D shaderTextureB;
uniform sampler2D cake_txt_inner;
uniform sampler2D cake_txt_outer;
uniform sampler2D lettuce_txt;

// camera
uniform vec3 cameraPos;

out vec4 fragmentColor;

vec4 colorA = texture(shaderTextureA, shaderTexCoord);
vec4 colorB = texture(shaderTextureB, shaderTexCoord);

vec4 cake_inner = texture(cake_txt_inner, shaderTexCoord);
vec4 cake_outer = texture(cake_txt_outer, shaderTexCoord);
vec4 lettuce = texture(lettuce_txt, shaderTexCoord);

uniform float t; // time
// vec3 lightPosition = vec3(clamp(abs(sin(t)), 0, 1) * 50, 7.0f, 7.0f);
vec3 lightColor = vec3(1.0f, 0.95f, 0.76f);
vec3 red = vec3(1.0f, 0.1f, 0.1f);
vec3 green = vec3(0.1f, 1.0f, 0.1f);
vec3 blue = vec3(0.1f, 0.1f, 1.0f);

uniform vec3 lightPosition;
// uniform vec3 lightColor;
uniform float specularity;

// candle

void main()
{
    // lightColor *= mix(red, green, abs(sin(t)));

    vec4 shade = vec4(shaderColor.x - 0.19f, shaderColor.y - 0.33f, shaderColor.z - 0.34f, 1.0f); // solid coloring shade, not lighting related
    vec4 darkerShade = vec4(shaderColor.x - 0.39f, shaderColor.y - 0.51f, shaderColor.z - 0.52f, 1.0f); // solid coloring shade, not lighting related
    vec3 red = vec3(shaderColor.x - 0.18f, shaderColor.y - 0.77f, shaderColor.z - 0.77f);

    // diffuse lighting 
    vec3 lightVector = normalize(lightPosition - worldSpacePosition); // l
    vec3 reNormalizeWorldNormal = normalize(worldSpaceNormal); // n
    vec3 diffuseColor = max(dot(reNormalizeWorldNormal, lightVector), 0) * lightColor; // Cd
    // ambient lighting
    float ambientStrength = 0.1f;
    vec3 ambientColor = ambientStrength * lightColor;
    // specular lighting
    // float reflection = lightVector - 2 * dot(reNormalizeWorldNormal, lightVector) * reNormalizeWorldNormal;
    float specularStrength = 0.5f;
    vec3 reflectionVector = reflect(-lightVector, reNormalizeWorldNormal);
    vec3 specularLight = specularStrength * pow(max(dot(reflectionVector, normalize(cameraPos - worldSpacePosition)), 0), specularity) * lightColor;

    // fragmentColor = vec4(((diffuseColor + ambientColor + specularLight) * (lightColor)), 1.0f);
    vec3 result = (ambientColor + diffuseColor + specularLight);

    fragmentColor = vec4(result, 1.0f);

    // adding textures and solid color shading
    if(shaderSide.y == 0) {
        fragmentColor = vec4(result, 1.0f) * colorA;
    }
    if(shaderSide.y == 1) {
        fragmentColor = vec4(result, 1.0f) * colorB;
    }
    if(shaderSide.x == 2) {
        fragmentColor = vec4(result, 1.0f) * colorB;
    }
    if(shaderSide.x == 3) {
        fragmentColor = vec4(result, 1.0f) * colorB; // removed multiply to darkerShade
    }
    if(shaderSide.x == 4) {
        fragmentColor = vec4(result, 1.0f) * cake_inner;
    }    
    if(shaderSide.x == 5) {
        fragmentColor = vec4(result, 1.0f) * cake_outer;
    } 
    if(shaderSide.x == 7) {
        if (lettuce.a < 0.7)
            discard;
        fragmentColor = vec4(result, 1.0f) * lettuce;
    }
    // fragmentColor = vec4(result, 0.1f);
}


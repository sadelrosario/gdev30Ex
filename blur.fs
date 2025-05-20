// #version 330 core
// out vec4 bloomColor;
// in vec2 texCoords;
// // in vec2 shaderSetter;

// uniform sampler2D screenTexture;
  
// uniform bool horizontal;
// float blurRadius = 6;
// const float weight[64] = float[] (0.026597, 0.026538, 0.026361, 0.026070, 0.025668, 0.025159, 0.024552, 0.023853, 0.023071, 0.022215, 0.021297, 0.020326, 0.019313, 0.018269, 0.017205, 0.016132, 0.015058, 0.013993, 0.012946, 0.011924, 0.010934, 0.009982, 0.009072, 0.008209, 0.007395, 0.006632, 0.005921, 0.005263, 0.004658, 0.004104, 0.003599, 0.003143, 0.002733, 0.002365, 0.002038, 0.001748, 0.001493, 0.001269, 0.001075, 0.000906, 0.000760, 0.000635, 0.000528, 0.000437, 0.000360, 0.000295, 0.000241, 0.000196, 0.000159, 0.000128, 0.000103, 0.000082, 0.000065, 0.000052, 0.000041, 0.000032, 0.000025, 0.000019, 0.000015, 0.000012, 0.000009, 0.000007, 0.000005, 0.000004);
// void main()
// {             
//     vec2 tex_offset = 1.0 / textureSize(screenTexture, 0); // gets size of single texel
//     vec3 result = texture(screenTexture, shaderTexCoord).rgb * weight[0]; // current fragment's contribution
//     if(horizontal)
//     {
//         for(int i = 1; i < blurRadius; ++i)
//         {
//             result += texture(screenTexture, shaderTexCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
//             result += texture(screenTexture, shaderTexCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
//         }
//     }
//     else
//     {
//         for(int i = 1; i < blurRadius; ++i)
//         {
//             result += texture(screenTexture, shaderTexCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
//             result += texture(screenTexture, shaderTexCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
//         }
//     }
//     bloomColor = vec4(result, 1.0);


// }

#version 330 core
out vec4 FragColor;
  
in vec2 texCoords;

uniform sampler2D screenTexture;
uniform bool horizontal;

// How far from the center to take samples from the fragment you are currently on
const int radius = 10;
// Keep it between 1.0f and 2.0f (the higher this is the further the blur reaches)
float spreadBlur = 2.0f;
float weights[radius];

void main()
{             
    // Calculate the weights using the Gaussian equation
    float x = 0.0f;
    for (int i = 0; i < radius; i++)
    {
        // Decides the distance between each sample on the Gaussian function
        if (spreadBlur <= 2.0f)
            x += 3.0f / radius;
        else
            x += 6.0f / radius;

        weights[i] = exp(-0.5f * pow(x / spreadBlur, 2.0f)) / (spreadBlur * sqrt(2 * 3.14159265f));
    }


    vec2 tex_offset = 1.0f / textureSize(screenTexture, 0);
    vec3 result = texture(screenTexture, texCoords).rgb * weights[0];

    // Calculate horizontal blur
    if(horizontal)
    {
        for(int i = 1; i < radius; i++)
        {
            // Take into account pixels to the right
            result += texture(screenTexture, texCoords + vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
            // Take into account pixels on the left
            result += texture(screenTexture, texCoords - vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
        }
    }
    // Calculate vertical blur
    else
    {
        for(int i = 1; i < radius; i++)
        {
            // Take into account pixels above
            result += texture(screenTexture, texCoords + vec2(0.0, tex_offset.y * i)).rgb * weights[i];
            // Take into account pixels below
            result += texture(screenTexture, texCoords - vec2(0.0, tex_offset.y * i)).rgb * weights[i];
        }
    }
    FragColor = vec4(result, 1.0f);
}
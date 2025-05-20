/******************************************************************************
 * This is a really simple fragment shader that simply sets the output fragment
 * color to yellow.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core

in vec3 shaderColor; // this output becomes the input in the fragment shader
in vec2 shaderTexCoord;
in vec2 shaderSetter;
// in vec3 bloomColor;
uniform sampler2D shaderTextureA;
uniform sampler2D shaderTextureB;
// uniform float t;
// vec2 transform = vec2(sin(t), sin(t));
layout (location = 0) out vec4 fragmentColor;
layout (location = 1) out vec4 bloomColor;

// uniform vec4 bloom = vec4(0, 0, 0, 1.0);
// uniform bool horizontal;
// uniform float weight[64] = float[] (0.026597, 0.026538, 0.026361, 0.026070, 0.025668, 0.025159, 0.024552, 0.023853, 0.023071, 0.022215, 0.021297, 0.020326, 0.019313, 0.018269, 0.017205, 0.016132, 0.015058, 0.013993, 0.012946, 0.011924, 0.010934, 0.009982, 0.009072, 0.008209, 0.007395, 0.006632, 0.005921, 0.005263, 0.004658, 0.004104, 0.003599, 0.003143, 0.002733, 0.002365, 0.002038, 0.001748, 0.001493, 0.001269, 0.001075, 0.000906, 0.000760, 0.000635, 0.000528, 0.000437, 0.000360, 0.000295, 0.000241, 0.000196, 0.000159, 0.000128, 0.000103, 0.000082, 0.000065, 0.000052, 0.000041, 0.000032, 0.000025, 0.000019, 0.000015, 0.000012, 0.000009, 0.000007, 0.000005, 0.000004);

void main()
{
   vec4 colorA = texture(shaderTextureA, shaderTexCoord);
   vec4 colorB = texture(shaderTextureB, shaderTexCoord);
   // fragmentColor = vec4(shaderColor, 1.0f) + colorA;
   if (shaderSetter.x == 1)
   {
      fragmentColor = vec4(shaderColor, 1.0f) + colorA;
   }
   else
   {
      fragmentColor = vec4(shaderColor, 1.0f) + colorB;
   }


	// Make the red lines of the lava brighter
	if (fragmentColor.r == 1)
		fragmentColor.r *= 5.0f;

	// Calculate brightness by adding up all the channels with different weights each
	float brightness = dot(fragmentColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
   if(fragmentColor.r == 1)
        bloomColor = vec4(fragmentColor.rgb, 1.0f); // has blur 
    else
        bloomColor = vec4(0.0f, 0.0f, 0.0f, 1.0f) + colorB; // no blur

   // float brightness = dot(fragmentColor.rgb, vec3(0.2126, 0.7152, 0.0722));
   // if(brightness > 0.15)
   // {
   //    bloomColor += vec4(fragmentColor.rgb, 1.0);
   // }
   // else
   // {
   //    bloomColor += vec4(0.0, 0.0, 0.0, 1.0);
   // }

   // fragmentColor += bloomColor;

   // vec2 tex_offset = 1.0 / textureSize(shaderTextureA, 0); // get size of single texel
   // // vec3 result = vec3(colorA.x * weight[0], colorA.y * weight[0], colorA.z * weight[0]); // current fragment's contribution
   // vec3 result = texture(shaderTextureA, shaderTexCoord).rgb * weight[0];
   // if(shaderSetter.x == 1) // horizontal blur
   // {
   //     for(int i = 0; i < 10; i++)
   //     {
   //          for(int i = 1; i < 5; i++)
   //          { 
   //         //vec2 TexCoord = vec2((shaderTexCoord.x + tex_offset.x) * i * weight[i], shaderTexCoord + 0.0 * weight[i]);
           
   //         result += vec3(colorA.x, colorA.y + tex_offset.x * float(i), colorA.z) * weight[i];
   //         result += vec3(colorA.x, colorA.y - tex_offset.x * float(i), colorA.z) * weight[i];

   //         // result += vec4(colorA, shaderTexCoord - vec2(tex_offset.x * i, 0.0).xyz * weight[i]);
   //          }
   //     }
       
   // }
   // if(shaderSetter.y == 1) // vertical blur
   // {
   //     for(int i = 1; i < 5; i++)
   //     {
   //       //   result += vec4(colorA, shaderTexCoord + vec2(tex_offset.y * i, 0.0)).xyz * weight[i];
   //       //   result += vec4(colorA, shaderTexCoord - vec2(tex_offset.y * i, 0.0)).xyz * weight[i];
   //       result += vec3(colorA.x, colorA.y, colorA.z + tex_offset.y * float(i)) * weight[i];
   //       result += vec3(colorA.x, colorA.y, colorA.z - tex_offset.y * float(i)) * weight[i];
   //     }
   
   // }
   // fragmentColor = vec4(result, 1.0f);
   // vec4 colorA = texture(shaderTextureA, shaderTexCoord);
   // vec4 colorB = texture(shaderTextureB, shaderTexCoord * transform);
   // fragmentColor = mix(colorA, colorB, abs(sin(t)));
}



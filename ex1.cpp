/******************************************************************************
 * This demo draws a triangle by defining its vertices in 3 dimensions
 * (the 3rd dimension is currently ignored and is just set to 0).
 *
 * The drawing is accomplished by:
 * - Uploading the vertices to the GPU using a Vertex Buffer Object (VBO).
 * - Specifying the vertices' format using a Vertex Array Object (VAO).
 * - Using a GLSL shader program (consisting of a simple vertex shader and a
 *   simple fragment shader) to actually draw the vertices as a triangle.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <gdev.h>
#include <math.h>
using namespace std;

// change this to your desired window attributes
#define WINDOW_WIDTH  1000
#define WINDOW_HEIGHT 1000
#define WINDOW_TITLE  "Hello Cloud Strife"
GLFWwindow *pWindow;

// define a vertex array to hold our vertices
float vertices[] =
{
    // -0.40f, -0.50f,  0.00f, 0.00f, 0.00f, 0.00f,
    //  0.40f, -0.50f,  0.00f, 0.00f, 0.00f, 0.00f,
    //  0.00f,  0.50f,  0.00f, 1.00f, 1.00f, 1.00f

    // hair (back)
     0.362f, -0.006f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
     0.281f, -0.227f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
     0.573f, -0.272f, 0.00f, 0.79f, 0.65f, 0.53f, 0.0f, 0.0f,

     0.324f, -0.050f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
     0.382f,  0.025f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
     0.554f, -0.119f, 0.00f, 0.79f, 0.65f, 0.53f, 0.0f, 0.0f,

     0.337f, -0.216f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
     0.236f, -0.271f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
     0.391f, -0.381f, 0.00f, 0.79f, 0.65f, 0.53f, 0.0f, 0.0f,

    // body
    -0.094f, -0.423f, 0.00f, 0.87f, 0.71f, 0.61f, 0.0f, 0.0f,
    -0.090f, -0.630f, 0.00f, 0.87f, 0.71f, 0.61f, 0.0f, 0.0f,
     0.129f, -0.401f, 0.00f, 0.87f, 0.71f, 0.61f, 0.0f, 0.0f,

    -0.091f, -0.624f, 0.00f, 0.87f, 0.71f, 0.61f, 0.0f, 0.0f,
     0.117f, -0.634f, 0.00f, 0.87f, 0.71f, 0.61f, 0.0f, 0.0f,
     0.119f, -0.404f, 0.00f, 0.87f, 0.71f, 0.61f, 0.0f, 0.0f,

    -0.104f, -0.516f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
    -0.106f, -0.634f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
     0.130f, -0.518f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,

    -0.104f, -0.628f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
     0.126f, -0.634f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
     0.130f, -0.518f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,

    -0.440f, -0.600f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
    -0.412f, -0.931f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
    -0.256f, -0.629f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,

    -0.777f, -0.925f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
    -0.396f, -0.931f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
    -0.440f, -0.600f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,

     0.317f, -0.627f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
     0.305f, -0.905f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
     0.642f, -0.918f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,

    -0.275f, -0.623f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
    -0.418f, -0.942f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
     0.321f, -0.628f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,

    -0.416f, -0.937f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
     0.383f, -0.919f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
     0.315f, -0.629f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,

     0.216f, -0.613f, 0.00, 0.61f, 0.62f, 0.66f, 0.0f, 0.0f,
     0.644f, -0.929f, 0.00, 0.61f, 0.62f, 0.66f, 0.0f, 0.0f,
     0.445f, -0.469f, 0.00, 0.61f, 0.62f, 0.66f, 0.0f, 0.0f,

     0.445f, -0.469f, 0.00, 0.61f, 0.62f, 0.66f, 0.0f, 0.0f,
     0.644f, -0.929f, 0.00, 0.61f, 0.62f, 0.66f, 0.0f, 0.0f,
     0.705f, -0.668f, 0.00, 0.61f, 0.62f, 0.66f, 0.0f, 0.0f,

    // face
     0.002f, -0.533f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
     0.002f, -0.093f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
    -0.150f, -0.413f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,

    -0.146f, -0.416f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
     0.003f, -0.098f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
    -0.263f, -0.302f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,

    -0.260f, -0.304f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
     0.006f, -0.105f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
    -0.291,   0.065f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,

     0.004f, -0.106f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
    -0.180f,  0.207f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
    -0.290f,  0.062f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,

    -0.392,  -0.172f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
    -0.275,  -0.081f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
    -0.336f, -0.050f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,

    -0.248f, -0.295f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
    -0.278f, -0.080f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
    -0.391f, -0.172f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,

     0.002f, -0.533f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
     0.153f, -0.413f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
     0.002f, -0.093f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,

     0.150f, -0.416f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
     0.267f, -0.302f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
     0.002f, -0.098f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,

     0.265f, -0.306f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
     0.295f,  0.065f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
     0.000f, -0.105f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,

     0.000f, -0.105f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
     0.294f,  0.062f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
     0.186f,  0.206f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,

     0.280f, -0.081f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
     0.397f, -0.173f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
     0.341f, -0.050f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,

     0.253f, -0.296f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
     0.396f, -0.172f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
     0.283f, -0.080f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,

     0.003f, -0.113f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
     0.190f,  0.200f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,
    -0.182f,  0.203f, 0.00f, 0.99f, 0.89f, 0.76f, 0.0f, 0.0f,

    // nose
     0.005f, -0.289f, 0.00f, 0.87f, 0.71f, 0.61f, 0.0f, 0.0f,
     0.040f, -0.244f, 0.00f, 0.87f, 0.71f, 0.61f, 0.0f, 0.0f,
     0.005f, -0.176f, 0.00f, 0.87f, 0.71f, 0.61f, 0.0f, 0.0f,

    // left eye (from viewer pov) - whites
    -0.209f, -0.076f, 0.00f, 1.00f, 1.00f, 1.00f, 0.0f, 0.0f,
    -0.075f, -0.160f, 0.00f, 1.00f, 1.00f, 1.00f, 0.0f, 0.0f,
    -0.101f, -0.091f, 0.00f, 1.00f, 1.00f, 1.00f, 0.0f, 0.0f,

    -0.199f, -0.155f, 0.00f, 1.00f, 1.00f, 1.00f, 0.0f, 0.0f,
    -0.076f, -0.159f, 0.00f, 1.00f, 1.00f, 1.00f, 0.0f, 0.0f,
    -0.204f, -0.072f, 0.00f, 1.00f, 1.00f, 1.00f, 0.0f, 0.0f,

    -0.199f, -0.154f, 0.00f, 1.00f, 1.00f, 1.00f, 0.0f, 0.0f,
    -0.198f, -0.072f, 0.00f, 1.00f, 1.00f, 1.00f, 0.0f, 0.0f,
    -0.253f, -0.085f, 0.00f, 1.00f, 1.00f, 1.00f, 0.0f, 0.0f,

    // iris LEFT
    -0.114f, -0.159f, 0.00f, 0.56f, 0.81f, 0.92f, 4.0f, 0.0f,
    -0.115f, -0.089f, 0.00f, 0.56f, 0.81f, 0.92f, 4.0f, 0.0f,
    -0.154f, -0.080f, 0.00f, 0.56f, 0.81f, 0.92f, 4.0f, 0.0f,

    -0.174f, -0.158f, 0.00f, 0.56f, 0.81f, 0.92f, 4.0f, 0.0f,
    -0.114f, -0.159f, 0.00f, 0.56f, 0.81f, 0.92f, 4.0f, 0.0f,
    -0.175f, -0.099f, 0.00f, 0.56f, 0.81f, 0.92f, 4.0f, 0.0f,

    -0.175f, -0.101f, 0.00f, 0.56f, 0.81f, 0.92f, 4.0f, 0.0f,
    -0.114f, -0.160f, 0.00f, 0.56f, 0.81f, 0.92f, 4.0f, 0.0f,
    -0.124f, -0.093f, 0.00f, 0.56f, 0.81f, 0.92f, 4.0f, 0.0f,

    // lashes LEFT
    -0.112f, -0.089f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
    -0.089f, -0.115f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
    -0.100f, -0.090f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,

    -0.134f, -0.085f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
    -0.100f, -0.099f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
    -0.102f, -0.092f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,

    -0.203f, -0.079f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
    -0.099f, -0.091f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
    -0.206f, -0.062f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,

    -0.252f, -0.093f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
    -0.200f, -0.078f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
    -0.206f, -0.063f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,

    -0.250f, -0.093f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
    -0.231f, -0.119f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
    -0.240f, -0.089f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,

    -0.229f, -0.058f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
    -0.216f, -0.074f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
    -0.203f, -0.064f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,

    -0.268f, -0.080f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
    -0.248f, -0.095f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
    -0.229f, -0.080f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,

    // // eyebrow LEFT (frown)
    // -0.291f,  0.021f, 0.00f, 0.79f, 0.65f, 0.53f, 1.0f,
    // -0.059f, -0.043f, 0.00f, 0.79f, 0.65f, 0.53f, 1.0f,
    // -0.089f, -0.013f, 0.00f, 0.79f, 0.65f, 0.53f, 1.0f,

    // // eyebrow RIGHT (frown)
    // 0.081f, -0.043f, 0.00f, 0.79f, 0.65f, 0.53f, 1.0f,
    // 0.313f,  0.021f, 0.00f, 0.79f, 0.65f, 0.53f, 1.0f,
    // 0.112f, -0.013f, 0.00f, 0.79f, 0.65f, 0.53f, 1.0f,

    // // mouth (frown)
    // -0.056, -0.367f, 0.00f, 0.87f, 0.71f, 0.61f, 2.0f,
    //  0.076, -0.368f, 0.00f, 0.87f, 0.71f, 0.61f, 2.0f,
    //  0.011, -0.357f, 0.00f, 0.87f, 0.71f, 0.61f, 2.0f,

    // left brow (happy)
    -0.292f, 0.020f, 0.00f, 0.79f, 0.65f, 0.53f, 1.0f, 0.0f,
    -0.050f, 0.020f, 0.00f, 0.79f, 0.65f, 0.53f, 1.0f, 1.0f, // animate brow bottom
    -0.087f, 0.040f, 0.00f, 0.79f, 0.65f, 0.53f, 1.0f, 2.0f, // animate brow top

     // right brow (happy)
     0.074f, 0.020f, 0.00f, 0.79f, 0.65f, 0.53f, 2.0f, 1.0f, // animate brow bottmm
     0.312f, 0.020f, 0.00f, 0.79f, 0.65f, 0.53f, 2.0f, 0.0f, 
     0.110f, 0.040f, 0.00f, 0.79f, 0.65f, 0.53f, 2.0f, 2.0f, // animate brow top

     // triangle mouth - (subtle) smile
    -0.056f, -0.360f, 0.00f, 0.87f, 0.71f, 0.61f, 3.0f, 1.0f, // animate left corner lip
     0.077f, -0.358f, 0.00f, 0.87f, 0.71f, 0.61f, 3.0f, 3.0f, // animate right corner lip 
     0.010f, -0.368f, 0.00f, 0.87f, 0.71f, 0.61f, 3.0f, 2.0f, // animate middle lip


    // right eye (whites)
     0.097f, -0.161f, 0.00f, 1.00f, 1.00f, 1.00f, 0.0f, 0.0f,
     0.232f, -0.075f, 0.00f, 1.00f, 1.00f, 1.00f, 0.0f, 0.0f,
     0.124f, -0.091f, 0.00f, 1.00f, 1.00f, 1.00f, 0.0f, 0.0f,

     0.099f, -0.159f, 0.00f, 1.00f, 1.00f, 1.00f, 0.0f, 0.0f,
     0.222f, -0.155f, 0.00f, 1.00f, 1.00f, 1.00f, 0.0f, 0.0f,
     0.228f, -0.072f, 0.00f, 1.00f, 1.00f, 1.00f, 0.0f, 0.0f,

     0.221f, -0.072f, 0.00f, 1.00f, 1.00f, 1.00f, 0.0f, 0.0f,
     0.221f, -0.155f, 0.00f, 1.00f, 1.00f, 1.00f, 0.0f, 0.0f,
     0.276f, -0.085f, 0.00f, 1.00f, 1.00f, 1.00f, 0.0f, 0.0f,

    // right iris
     0.137f, -0.090f, 0.00f, 0.56f, 0.81f, 0.92f, 4.0f, 0.0f,
     0.137f, -0.159f, 0.00f, 0.56f, 0.81f, 0.92f, 4.0f, 0.0f,
     0.177f, -0.081f, 0.00f, 0.56f, 0.81f, 0.92f, 4.0f, 0.0f,

     0.137f, -0.159f, 0.00f, 0.56f, 0.81f, 0.92f, 4.0f, 0.0f,
     0.197f, -0.158f, 0.00f, 0.56f, 0.81f, 0.92f, 4.0f, 0.0f,
     0.199f, -0.098f, 0.00f, 0.56f, 0.81f, 0.92f, 4.0f, 0.0f,

     0.147f, -0.094f, 0.00f, 0.56f, 0.81f, 0.92f, 4.0f, 0.0f, 
     0.136f, -0.160f, 0.00f, 0.56f, 0.81f, 0.92f, 4.0f, 0.0f,
     0.198f, -0.100f, 0.00f, 0.56f, 0.81f, 0.92f, 4.0f, 0.0f,

    // right lashes 
     0.122f, -0.090f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
     0.111f, -0.116f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
     0.135f, -0.089f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,

     0.124f, -0.092f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
     0.123f, -0.100f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
     0.158f, -0.085f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,

     0.122f, -0.091f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
     0.226f, -0.079f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
     0.230f, -0.062f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,

     0.222f, -0.078f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
     0.275f, -0.093f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
     0.229f, -0.063f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
 
     0.263f, -0.089f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
     0.254f, -0.120f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
     0.274f, -0.093f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,

     0.226f, -0.064f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
     0.239f, -0.074f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
     0.252f, -0.058f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,

     0.251f, -0.080f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
     0.271f, -0.095f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,
     0.291f, -0.080f, 0.00f, 0.33f, 0.32f, 0.36f, 0.0f, 0.0f,

    // hair
    -0.055f,  0.208f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
     0.353f,  0.203f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
    -0.200f,  0.968f, 0.00f, 0.79f, 0.65f, 0.53f, 0.0f, 0.0f,

     0.391f,  0.223f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
     0.323f, -0.028f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
     0.645f,  0.084f, 0.00f, 0.79f, 0.65f, 0.53f, 0.0f, 0.0f,
 
     0.182f,  0.221f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
     0.361f,  0.249f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
     0.199f,  0.663f, 0.00f, 0.79f, 0.65f, 0.53f, 0.0f, 0.0f,

    -0.387f,  0.091f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
    -0.237f,  0.219f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
    -0.296f, -0.537f, 0.00f, 0.79f, 0.65f, 0.53f, 0.0f, 0.0f,

     0.036f,  0.387f, 0.00f, 0.79f, 0.65f, 0.53f, 0.0f, 0.0f,
    -0.001f,  0.203f, 0.00f, 0.79f, 0.65f, 0.53f, 0.0f, 0.0f,
     0.173f,  0.185f, 0.00f, 0.79f, 0.65f, 0.53f, 0.0f, 0.0f,

     0.134f,  0.220f, 0.00f, 0.79f, 0.65f, 0.53f, 0.0f, 0.0f,
     0.263f,  0.088f, 0.00f, 0.79f, 0.65f, 0.53f, 0.0f, 0.0f,
     0.287f,  0.306f, 0.00f, 0.79f, 0.65f, 0.53f, 0.0f, 0.0f,

     0.272f,  0.310f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
     0.455f,  0.420f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
     0.239f, -0.582f, 0.00f, 0.79f, 0.65f, 0.53f, 0.0f, 0.0f,

     0.036f,  0.210f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
    -0.093f,  0.466f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
    -0.690f,  0.354f, 0.00f, 0.79f, 0.65f, 0.53f, 0.0f, 0.0f,

     0.017f,  0.219f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
    -0.274f,  0.352f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
    -0.705f, -0.114f, 0.00f, 0.79f, 0.65f, 0.53f, 0.0f, 0.0f,

    -0.113f,  0.078f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
    -0.059f,  0.434f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
    -0.627f, -0.238f, 0.00f, 0.79f, 0.65f, 0.53f, 0.0f, 0.0f,

     0.037f,  0.202f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
     0.037f,  0.395f, 0.00f, 0.92f, 0.82f, 0.57f, 0.0f, 0.0f,
    -0.444f, -0.271f, 0.00f, 0.79f, 0.65f, 0.53f, 0.0f, 0.0f,
};


// define OpenGL object IDs to represent the vertex array and the shader program in the GPU
GLuint vao;         // vertex array object (stores the render state for our vertex array)
GLuint vbo;         // vertex buffer object (reserves GPU memory for our vertex array)
GLuint shader;      // combined vertex and fragment shader

// called by the main function to do initial setup, such as uploading vertex
// arrays, shader programs, etc.; returns true if successful, false otherwise
bool setup()
{
    // generate the VAO and VBO objects and store their IDs in vao and vbo, respectively
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    // bind the newly-created VAO to make it the current one that OpenGL will apply state changes to
    glBindVertexArray(vao);

    // upload our vertex array data to the newly-created VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // on the VAO, register the current VBO with the following vertex attribute layout:
    // - layout location 0...
    // - ... shall consist of 3 GL_FLOATs (corresponding to x, y, and z coordinates)
    // - ... its values will NOT be normalized (GL_FALSE)
    // - ... the stride length is the number of bytes of all 3 floats of each vertex (hence, 3 * sizeof(float))
    // - ... and we start at the beginning of the array (hence, (void*) 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0); // position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float))); // color
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float))); // anim

    // enable the newly-created layout location 0;
    // this shall be used by our vertex shader to read the vertex's x, y, and z
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);



    // important: if you have more vertex arrays to draw, make sure you separately define them
    // with unique VAO and VBO IDs, and follow the same process above to upload them to the GPU

    // load our shader program
    shader = gdevLoadShader("ex1.vs", "ex1.fs");
    if (! shader)
        return false;

    return true;
}

// called by the main function to do rendering per frame
void render()
{
    // clear the whole frame
    glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // setting timing for animations
    int t = glfwGetTime(); // int for time so we can modulo
    float glow = fabs((cos(glfwGetTime()) / 3.0f)) / 2.0f + 0.5f;
    float s = glfwGetTime();
    float a = 0;
    float c = 0;
    float xbottom = 0.005f;
    float ybottom = 0.063f;
    float xtop = 0.009f;
    float ytop = 0.053f;
    float frownTime = sin(t * 0.7 - 1.57079632f);
    float r = 0.25f;
    float g = 0.86f;
    float b = 0.68f;

    if (frownTime < 0.3) {
        glow = 1;
    }
    
    if (frownTime > 0.527f) { // eye color change to 0.25f, 0.86f, 0.68 from 0.81f, 0.92f, 4.0f,
        c = 1;
        cout << "CHOLOR";
    }

    if (frownTime > 0.5f){ // expression change
        a = 2;
    } else {
        glow = 1;
    }

    

    // add unifoms
    glUniform1f(glGetUniformLocation(shader, "glow"), glow);
    glUniform1f(glGetUniformLocation(shader, "seconds"), s);
    glUniform1f(glGetUniformLocation(shader, "animate"), a);
    glUniform1f(glGetUniformLocation(shader, "color"), c);
    glUniform4f(glGetUniformLocation(shader, "browChange"), xbottom, ybottom, xtop, ytop);
    glUniform1f(glGetUniformLocation(shader, "frown"), frownTime);
    glUniform3f(glGetUniformLocation(shader, "colorChange"), r, g, b);




    // using our shader program...
    glUseProgram(shader);

    // ... draw our triangles
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / (3 * sizeof(float)));
}

/*****************************************************************************/

// handler called by GLFW when there is a keyboard event
void handleKeys(GLFWwindow* pWindow, int key, int scancode, int action, int mode)
{
    // pressing Esc closes the window
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(pWindow, GL_TRUE);
}

// handler called by GLFW when the window is resized
void handleResize(GLFWwindow* pWindow, int width, int height)
{
    // tell OpenGL to do its drawing within the entire "client area" (area within the borders) of the window
    glViewport(0, 0, width, height);
}

// main function
int main(int argc, char** argv)
{
    // initialize GLFW and ask for OpenGL 3.3 core
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // create a GLFW window with the specified width, height, and title
    pWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (! pWindow)
    {
        // gracefully terminate if we cannot create the window
        std::cout << "Cannot create the GLFW window.\n";
        glfwTerminate();
        return -1;
    }

    // make the window the current context of subsequent OpenGL commands,
    // and enable vertical sync and aspect-ratio correction on the GLFW window
    glfwMakeContextCurrent(pWindow);
    glfwSwapInterval(1);
    glfwSetWindowAspectRatio(pWindow, WINDOW_WIDTH, WINDOW_HEIGHT);

    // set up callback functions to handle window system events
    glfwSetKeyCallback(pWindow, handleKeys);
    glfwSetFramebufferSizeCallback(pWindow, handleResize);

    // don't miss any momentary keypresses
    glfwSetInputMode(pWindow, GLFW_STICKY_KEYS, GLFW_TRUE);

    // initialize GLAD, which acts as a library loader for the current OS's native OpenGL library
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // if our initial setup is successful...
    if (setup())
    {
        // do rendering in a loop until the user closes the window
        while (! glfwWindowShouldClose(pWindow))
        {
            // render our next frame
            // (by default, GLFW uses double-buffering with a front and back buffer;
            // all drawing goes to the back buffer, so the frame does not get shown yet)
            render();

            // swap the GLFW front and back buffers to show the next frame
            glfwSwapBuffers(pWindow);

            // process any window events (such as moving, resizing, keyboard presses, etc.)
            glfwPollEvents();
        }
    }

    // gracefully terminate the program
    glfwTerminate();
    return 0;
}

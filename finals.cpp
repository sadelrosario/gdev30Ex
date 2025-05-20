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

// change this to your desired window attributes
#define WINDOW_WIDTH  1000
#define WINDOW_HEIGHT 1000
#define WINDOW_TITLE  "PIPIPI LOAF"


using namespace std;

GLFWwindow *pWindow;

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  7.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

glm::vec3 lightPosition = glm::vec3(7.0f, 7.0f, 50.0f);
float specularity = 32;

// set mouse controls
bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0f;
float lastY =  600.0f / 2.0f;
float fov   =  45.0f;

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float mapTexture(float coordinate) {
    return (coordinate / 2) + 0.5f;
}

glm::vec3 YawBack(float x, float y, float z) {
    glm::vec3 vect = glm::vec3(x, y, z);
    float pi = 3.14159;
    glm::mat3 matrix = glm::mat3(
        cos(pi), 0, sin(pi),
        0, 1, 0,
        -sin(pi), 0, cos(pi)
    );
    glm::vec3 yawed = matrix * vect; 
    return yawed;
}

glm::vec2 YawTex(float x, float y) {
    glm::vec2 vect = glm::vec2(x, y);
    float pi = 3.14159;
    glm::mat2 matrix = glm::mat2(
        cos(pi), sin(pi),
        -sin(pi), cos(pi)
    );
    glm::vec2 yawed = matrix * vect; 
    return yawed;
}

glm::vec3 norm(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    glm:: vec3 normalVector = glm::cross((b-a),(c-a));
    return normalVector;
}

void smoothNorm(float* verts, int rangeMin, int rangeMax, int steps) {
    // range = get vertices that you only want to smoothen
    // get range by index number (e.g, getting the first vertex would mean getting vertex 0)
    // range is inclusive
    // cout << "WE ARE ENTERING THE SMOOTHING FUNCTION\n";
    glm::vec3 vertCumSum = glm::vec3(0.0f, 0.0f, 0.0f);
    int totalVerts = 0; // range of smoothing (start with one vertex)
    
    for(int j = rangeMin; j <= rangeMax; j += 3) {
        if (verts[(j)*steps+9] == 3.0f) {
            // get current vertex in the range
            // cout << verts[j*steps] << ", " << verts[j*steps+1] << ", " << verts[j*steps+2] << endl;
            // keep adding the normals of the vertices
            vertCumSum += glm::vec3(verts[(j)*steps+10], verts[(j)*steps+11], verts[(j)*steps+12]); 
            // track how much vertices already traversed in range
            totalVerts ++;
        }
    }    

    // printf("total verts: %d\n", totalVerts);

    // normalize the average of the normals (OUTSIDE FORLOOP)
    glm::vec3 average = normalize(vertCumSum/(float)totalVerts);
    // cout << "average norm: " << average.x << ", " << average.y << ", " << average.z << endl;
    // int ctr = 0;
    for(int i = rangeMin; i <= rangeMax; i += 3) {
        if (verts[(i)*steps+9] == 3.0f) {
            for (int k = 0; k < 3; k++) {
                verts[(i+k)*steps+10] = average.x; 
                verts[(i+k)*steps+11] = average.y;
                verts[(i+k)*steps+12] = average.z;
            }
        }
        // ctr++;
    }
    // cout << "ctr: " << ctr << endl;

    // for(int i = rangeMin; i <= rangeMax; i++) {
    //     verts[i*steps+10] = average.x; 
    //     verts[i*steps+11] = average.y;
    //     verts[i*steps+12] = average.z;
    // }

    // if(checker == 1) {
    //     for(int i = rangeMin; i <= rangeMax; i++) {
    //         // glm::vec3 currentV(verts[i*steps], verts[i*steps+1], verts[i*steps+2]);
    //         verts[i*steps+10] = average.x; 
    //         verts[i*steps+11] = average.y;
    //         verts[i*steps+12] = average.z;
    //     }
    // }

    // if(checker == 0) {
    //     for(int i = rangeMin; i <= rangeMax; i++) {
    //         // glm::vec3 currentV(verts[i*steps], verts[i*steps+1], verts[i*steps+2]);
    //         verts[i*steps+10] = average.x; 
    //         verts[i*steps+11] = average.y;
    //         verts[i*steps+12] = average.z;
    //     }
    // }
    // // now replace every norm of every vertex in range with the smoothen norms
    // for(int i = rangeMin; i <= rangeMax; i++) {
    //     // glm::vec3 currentV(verts[i*steps], verts[i*steps+1], verts[i*steps+2]);
    //     verts[i*steps+10] = -average.x; 
    //     verts[i*steps+11] = -average.y;
    //     verts[i*steps+12] = -average.z;
    // }

//     printf("vert cumsum is: (%f, %f, %f)\n", vertCumSum.x, vertCumSum.y, vertCumSum.z);
//     cout << "range count: " << rangeCount << endl << endl;
}

#define THREEDVERT(x, y, z) YawBack(x, y, z)[0], YawBack(x, y, z)[1], YawBack(x, y, z)[2]
#define TWODVERT(x, y) mapTexture(YawTex(x, y)[0]), mapTexture(YawTex(x, y)[1])

// define a vertex array to hold our vertices
float vertices[] =
{
    // position (x, y, z)  color (r, g, b)   texture coordinates (s, t)   side determinant (a, b)    normal (x, y, z)

   // base = 3
   // 1
  THREEDVERT(-0.383f, 0.082f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.383f), mapTexture( 0.082f), 0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
  THREEDVERT(0.432f,  0.082f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture( 0.432f), mapTexture( 0.082f), 0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
  THREEDVERT(0.024f,  0.303f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture( 0.024f), mapTexture( 0.303f), 0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
   // 2
  THREEDVERT(-0.383f,  0.082f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.383f), mapTexture( 0.082f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
  THREEDVERT(-0.383f, -0.383f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.383f), mapTexture(-0.383f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
  THREEDVERT( 0.432f,  0.082f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture( 0.432f), mapTexture( 0.082f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   // 3
  THREEDVERT(-0.383f, -0.383f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.383f), mapTexture(-0.383f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
  THREEDVERT( 0.432f, -0.383f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture( 0.432f), mapTexture(-0.383f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
  THREEDVERT( 0.432f,  0.082f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture( 0.432f), mapTexture( 0.082f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,

  // left ear = 4
   // 4
  THREEDVERT(-0.462f,  0.253f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.462f), mapTexture( 0.253f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
  THREEDVERT(-0.383f,  0.082f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.383f), mapTexture( 0.082f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
  THREEDVERT(-0.351f,  0.439f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.351f), mapTexture( 0.439f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   // 5
  THREEDVERT(-0.351f,  0.439f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.351f), mapTexture( 0.439f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
  THREEDVERT(-0.383f,  0.082f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.383f), mapTexture( 0.082f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
  THREEDVERT(-0.158f,  0.167f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.158f), mapTexture( 0.167f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   // 6
  THREEDVERT(-0.351f,  0.439f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.351f), mapTexture( 0.439f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
  THREEDVERT(-0.158f,  0.167f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.158f), mapTexture( 0.167f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
  THREEDVERT(-0.131f,  0.450f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.131f), mapTexture( 0.450f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   // 7
  THREEDVERT(-0.131f,  0.450f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.131f), mapTexture( 0.450f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
  THREEDVERT(-0.158f,  0.167f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.158f), mapTexture( 0.167f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
  THREEDVERT( 0.024f,  0.303f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture( 0.032f), mapTexture( 0.291f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,

  // right ear = 4
   // 8 (tip)
  THREEDVERT(0.396f,  0.439f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture( 0.396f), mapTexture( 0.439f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
  THREEDVERT(0.432f,  0.082f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture( 0.432f), mapTexture( 0.082f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
  THREEDVERT(0.505f,  0.253f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture( 0.505f), mapTexture( 0.253f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   // 9
  THREEDVERT(0.200f,  0.166f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture( 0.200f), mapTexture( 0.166f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
  THREEDVERT(0.432f,  0.082f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture( 0.432f), mapTexture( 0.077f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
  THREEDVERT(0.396f,  0.439f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture( 0.396f), mapTexture( 0.439f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   // 10
  THREEDVERT(0.176f,  0.450f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture( 0.176f), mapTexture( 0.450f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
  THREEDVERT(0.200f,  0.166f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture( 0.200f), mapTexture( 0.166f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
  THREEDVERT(0.396f,  0.439f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture( 0.396f), mapTexture( 0.439f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   // 11
  THREEDVERT(0.176f,  0.450f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture( 0.176f), mapTexture( 0.450f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
  THREEDVERT(0.024f,  0.303f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture( 0.011f), mapTexture( 0.290f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
  THREEDVERT(0.207f,  0.163f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture( 0.207f), mapTexture( 0.163f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,

    // FRONT
    // base = 3
   // 12
    -0.383f, 0.082f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.383f), mapTexture( 0.082f), 0.0f, 0.0f,    0.0f, 0.0f, 1.0f,
    0.432f,  0.082f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture( 0.432f), mapTexture( 0.082f), 0.0f, 0.0f,    0.0f, 0.0f, 1.0f,
    0.024f,  0.303f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture( 0.024f), mapTexture( 0.303f), 0.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   // 13
   -0.383f,  0.082f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.383f), mapTexture( 0.082f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   -0.383f, -0.383f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.383f), mapTexture(-0.383f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
    0.432f,  0.082f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture( 0.432f), mapTexture( 0.082f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   // 14
   -0.383f, -0.383f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.383f), mapTexture(-0.383f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
    0.432f, -0.383f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture( 0.432f), mapTexture(-0.383f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
    0.432f,  0.082f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture( 0.432f), mapTexture( 0.082f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,

   // left ear = 4
   // 15
   -0.462f,  0.253f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.462f), mapTexture( 0.253f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   -0.383f,  0.082f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.383f), mapTexture( 0.082f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   -0.351f,  0.439f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.351f), mapTexture( 0.439f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   // 16
   -0.351f,  0.439f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.351f), mapTexture( 0.439f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   -0.383f,  0.082f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.383f), mapTexture( 0.082f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   -0.158f,  0.167f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.158f), mapTexture( 0.167f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   // 17
   -0.351f,  0.439f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.351f), mapTexture( 0.439f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   -0.158f,  0.167f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.158f), mapTexture( 0.167f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   -0.131f,  0.450f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.131f), mapTexture( 0.450f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   // 18
   -0.131f,  0.450f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.131f), mapTexture( 0.450f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   -0.158f,  0.167f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.158f), mapTexture( 0.167f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
    0.024f,  0.303f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture( 0.032f), mapTexture( 0.291f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,

   // right ear = 4
   // 19
    0.396f,  0.439f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture( 0.396f), mapTexture( 0.439f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
    0.432f,  0.082f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture( 0.432f), mapTexture( 0.082f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
    0.505f,  0.253f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture( 0.505f), mapTexture( 0.253f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   // 20
    0.200f,  0.166f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture( 0.200f), mapTexture( 0.166f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
    0.432f,  0.082f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture( 0.432f), mapTexture( 0.077f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
    0.396f,  0.439f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture( 0.396f), mapTexture( 0.439f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   // 21
    0.176f,  0.450f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture( 0.176f), mapTexture( 0.450f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
    0.200f,  0.166f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture( 0.200f), mapTexture( 0.166f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
    0.396f,  0.439f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture( 0.396f), mapTexture( 0.439f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
   // 22
    0.176f,  0.450f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture( 0.176f), mapTexture( 0.450f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
    0.024f,  0.303f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture( 0.011f), mapTexture( 0.290f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
    0.207f,  0.163f,  1.00f, 1.0f, 1.0f, 1.0f, mapTexture( 0.207f), mapTexture( 0.163f), 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,

    // DEPTH (THE SIDES)

    // (apple) bottom (jeans) = 2
   // 23
   THREEDVERT(-0.383f, -0.383f, 1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.383f), mapTexture(-0.383f), 3.0f, 1.0f,     0.0f, 0.0f, 1.0f,
    0.432f, -0.383f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT( 0.432f, -0.383f), 3.0f, 1.0f,                               0.0f, 0.0f, 1.0f,
   -0.383f, -0.383f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(-0.383f, -0.383f), 3.0f, 1.0f,                               0.0f, 0.0f, 1.0f,
   // 24
   -0.383f, -0.383f,  1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(-0.383f, -0.383f), 3.0f, 1.0f,                              0.0f, 0.0f, 1.0f,
   THREEDVERT( 0.432f, -0.383f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture( 0.432f), mapTexture(-0.383f), 3.0f, 1.0f,    0.0f, 0.0f, 1.0f,
   THREEDVERT(-0.383f, -0.383f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.383f), mapTexture(-0.383f), 3.0f, 1.0f,    0.0f, 0.0f, 1.0f,

    // right base side = 2
   // 25
    -0.383f, -0.383f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(-0.383f, -0.383f), 2.0f, 1.0f,                              0.0f, 0.0f, 1.0f,
    THREEDVERT(0.432f,  0.082f,  1.00f), 1.0f, 1.0f, 1.0f, TWODVERT(0.432f,  0.082f), 2.0f, 1.0f,                   0.0f, 0.0f, 1.0f,
    THREEDVERT(0.432f, -0.383f,  1.00f), 1.0f, 1.0f, 1.0f, TWODVERT(0.432f, -0.383f), 2.0f, 1.0f,                   0.0f, 0.0f, 1.0f,
   // 26
    -0.383f, -0.383f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(-0.383f, -0.383f), 2.0f, 1.0f,                              0.0f, 0.0f, 1.0f,
    -0.383f,  0.082f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(-0.383f,  0.082f), 2.0f, 1.0f,                              0.0f, 0.0f, 1.0f,
    THREEDVERT(0.432f,  0.082f,  1.00f), 1.0f, 1.0f, 1.0f, TWODVERT(0.432f,  0.082f), 2.0f, 1.0f,                   0.0f, 0.0f, 1.0f,

    // right ear side = 4
    // 27
    THREEDVERT(0.432f,  0.082f,  1.00f), 1.0f, 1.0f, 1.0f, TWODVERT(0.432f,  0.082f), 3.0f, 3.0f,       0.0f, 0.0f, 1.0f,
    -0.383f,  0.082f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(-0.383f,  0.082f), 3.0f, 1.0f,                  0.0f, 0.0f, 1.0f,
    THREEDVERT(0.505f,  0.253f,  1.00f), 1.0f, 1.0f, 1.0f, TWODVERT(0.505f,  0.253f), 3.0f, 3.0f,       0.0f, 0.0f, 1.0f,
    // 28
    THREEDVERT(0.505f,  0.253f,  1.00f), 1.0f, 1.0f, 1.0f, TWODVERT(0.505f,  0.253f), 3.0f, 3.0f,       0.0f, 0.0f, 1.0f,
    -0.383f,  0.082f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(-0.383f,  0.082f), 3.0f, 3.0f,                  0.0f, 0.0f, 1.0f,
    -0.462f,  0.253f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(-0.462f,  0.253f), 3.0f, 3.0f,                  0.0f, 0.0f, 1.0f, //middle edge
    // 29
    THREEDVERT(0.505f,  0.253f,  1.00f), 1.0f, 1.0f, 1.0f,TWODVERT(0.505f,  0.253f), 2.0f, 3.0f,        0.0f, 0.0f, 1.0f,
    -0.462f,  0.253f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(-0.462f,  0.253f), 2.0f, 3.0f,                  0.0f, 0.0f, 1.0f, //middle edge
    -0.351f,  0.439f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(-0.351f,  0.439f), 2.0f, 3.0f,                  0.0f, 0.0f, 1.0f,
    // 30
    THREEDVERT(0.396f,  0.439f,  1.00f), 1.0f, 1.0f, 1.0f, TWODVERT(0.396f,  0.439f), 2.0f, 3.0f,       0.0f, 0.0f, 1.0f,
    THREEDVERT(0.505f,  0.253f,  1.00f), 1.0f, 1.0f, 1.0f, TWODVERT(0.505f,  0.253f), 2.0f, 3.0f,       0.0f, 0.0f, 1.0f,
    -0.351f,  0.439f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(-0.351f,  0.439f), 2.0f, 3.0f,                  0.0f, 0.0f, 1.0f,

    // top side = 8
   // 31
    THREEDVERT(0.176f,  0.450f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(0.176f), mapTexture(0.450f), 2.0f, 3.0f,     0.0f, 0.0f, 1.0f,
    THREEDVERT(0.396f,  0.439f,  1.00f), 1.0f, 1.0f, 1.0f, TWODVERT(0.396f, 0.439f), 2.0f, 3.0f,     0.0f, 0.0f, 1.0f,
    -0.351f,  0.439f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(-0.351f,  0.439f), 2.0f, 3.0f,                              0.0f, 0.0f, 1.0f,
    // THREEDVERT(0.176f,  0.450f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(0.176f), mapTexture( 0.450f), 2.0f, 1.0f,     0.0f, 0.0f, 1.0f,
    // THREEDVERT(0.396f,  0.439f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(0.396f), mapTexture( 0.439f), 2.0f, 1.0f,     0.0f, 0.0f, 1.0f,
    // -0.351f,  0.439f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(-0.351f,  0.439f), 2.0f, 1.0f,                              0.0f, 0.0f, 1.0f,
   // 32
    THREEDVERT(0.176f,  0.450f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(0.176f), mapTexture( 0.450f), 2.0f, 3.0f,     0.0f, 0.0f, 1.0f,
    -0.351f,  0.439f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(-0.351f,  0.439f), 2.0f, 3.0f,                              0.0f, 0.0f, 1.0f,
    -0.131f,  0.450f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(-0.131f,  0.450f), 2.0f, 3.0f,                              0.0f, 0.0f, 1.0f,
   // 33
    THREEDVERT(0.024f,  0.303f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(0.024f), mapTexture( 0.303f), 3.0f, 3.0f,     0.0f, 0.0f, 1.0f,
    THREEDVERT(0.176f,  0.450f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(0.176f), mapTexture( 0.450f), 3.0f, 3.0f,     0.0f, 0.0f, 1.0f,
    -0.131f,  0.450f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(-0.131f,  0.450f), 3.0f, 3.0f,                              0.0f, 0.0f, 1.0f,
   // 34
    THREEDVERT(0.024f,  0.303f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(0.024f), mapTexture( 0.303f), 3.0f, 3.0f,     0.0f, 0.0f, 1.0f,
    -0.131f,  0.450f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(-0.131f,  0.450f), 3.0f, 3.0f,                              0.0f, 0.0f, 1.0f,
     0.024f,  0.303f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT( 0.024f,  0.303f), 3.0f, 3.0f,                              0.0f, 0.0f, 1.0f,

   // 35
    THREEDVERT(-0.131f,  0.450f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.131f), mapTexture( 0.450f), 3.0f, 3.0f,       0.0f, 0.0f, 1.0f,
    THREEDVERT( 0.024f,  0.303f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture( 0.024f), mapTexture( 0.303f), 3.0f, 3.0f,       0.0f, 0.0f, 1.0f,
    0.024f,  0.303f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(0.024f,  0.303f), 3.0f, 3.0f,                                    0.0f, 0.0f, 1.0f,
   // 36
    THREEDVERT(-0.131f,  0.450f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.131f), mapTexture( 0.450f), 3.0f, 3.0f,       0.0f, 0.0f, 1.0f,
     0.024f,  0.303f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT( 0.024f,  0.303f), 3.0f, 3.0f,                                  0.0f, 0.0f, 1.0f,
     0.176f,  0.450f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT( 0.176f,  0.450f), 3.0f, 3.0f,                                  0.0f, 0.0f, 1.0f,
   // 37
    THREEDVERT(-0.351f,  0.439f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.351f), mapTexture( 0.439f), 2.0f, 3.0f,       0.0f, 0.0f, 1.0f,
    THREEDVERT(-0.131f,  0.450f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.131f), mapTexture( 0.450f), 2.0f, 3.0f,       0.0f, 0.0f, 1.0f,
    0.176f,  0.450f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(0.176f,  0.450f), 2.0f, 3.0f,                                    0.0f, 0.0f, 1.0f,
   // 38
    THREEDVERT(-0.351f,  0.439f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.351f), mapTexture( 0.439f), 2.0f, 3.0f,       0.0f, 0.0f, 1.0f,
    0.176f,  0.450f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(0.176f, 0.450f), 2.0f, 3.0f,                                    0.0f, 0.0f, 1.0f,
    0.396f,  0.439f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(0.396f,  0.439f), 2.0f, 3.0f,                                    0.0f, 0.0f, 1.0f,

    // THREEDVERT(-0.351f,  0.439f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.351f), mapTexture( 0.439f), 2.0f, 1.0f,       0.0f, 0.0f, 1.0f,
    // 0.176f,  0.450f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(0.176f,  0.450f), 2.0f, 1.0f,                                    0.0f, 0.0f, 1.0f,
    // 0.396f,  0.439f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(0.396f,  0.439f), 2.0f, 1.0f,  0.0f, 0.0f, 1.0f,

    // left ear side = 4
   // 39
   THREEDVERT(-0.462f,  0.253f,  1.00f), 1.0f, 1.0f, 1.0f, TWODVERT(-0.462f,  0.253f), 2.0f, 3.0f,      0.0f, 0.0f, 1.0f,
   THREEDVERT(-0.351f,  0.439f,  1.00f), 1.0f, 1.0f, 1.0f, mapTexture(-0.351f), mapTexture( 0.439f), 2.0f, 3.0f,      0.0f, 0.0f, 1.0f,
   0.396f,  0.439f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(0.396f,  0.439f), 2.0f, 3.0f,                     0.0f, 0.0f, 1.0f,
//       THREEDVERT(-0.462f,  0.253f,  1.00f), 1.0f, 1.0f, 1.0f, TWODVERT(-0.462f,  0.253f), 2.0f, 1.0f,      0.0f, 0.0f, 1.0f,
//    THREEDVERT(-0.351f,  0.439f,  1.00f), 1.0f, 1.0f, 1.0f, TWODVERT(-0.351f,  0.439f), 2.0f, 1.0f,      0.0f, 0.0f, 1.0f,
//    0.396f,  0.439f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(0.396f,  0.439f), 2.0f, 1.0f,                     0.0f, 0.0f, 1.0f,
   // 40
   THREEDVERT(-0.462f,  0.253f,  1.00f), 1.0f, 1.0f, 1.0f, TWODVERT(-0.462f,  0.253f), 2.0f, 3.0f,      0.0f, 0.0f, 1.0f,
   0.396f,  0.439f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(0.396f,  0.439f), 2.0f, 3.0f,                     0.0f, 0.0f, 1.0f,
   0.505f,  0.253f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(0.505f,  0.253f), 2.0f, 3.0f,                     0.0f, 0.0f, 1.0f,
   // 41
   THREEDVERT(-0.383f,  0.082f,  1.00f), 1.0f, 1.0f, 1.0f, TWODVERT(-0.383f,  0.082f), 3.0f, 3.0f,      0.0f, 0.0f, 1.0f,
   THREEDVERT(-0.462f,  0.253f,  1.00f), 1.0f, 1.0f, 1.0f, TWODVERT(-0.462f,  0.253f), 3.0f, 3.0f,      0.0f, 0.0f, 1.0f,
   0.505f,  0.253f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(0.505f,  0.253f), 3.0f, 3.0f,                     0.0f, 0.0f, 1.0f,
   // 42
   THREEDVERT(-0.383f,  0.082f,  1.00f), 1.0f, 1.0f, 1.0f, TWODVERT(-0.383f,  0.082f), 3.0f, 3.0f,      0.0f, 0.0f, 1.0f,
   0.505f,  0.253f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(0.505f,  0.253f), 3.0f, 3.0f,                     0.0f, 0.0f, 1.0f,
   0.432f,  0.082f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(0.432f,  0.082f), 3.0f, 3.0f,                     0.0f, 0.0f, 1.0f,

    // left base side = 2
   // 43
   0.432f,  0.082f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(0.432f,  0.082f), 2.0f, 1.0f,                     0.0f, 0.0f, 1.0f,
   THREEDVERT(-0.383f, -0.383f,  1.00f), 1.0f, 1.0f, 1.0f, TWODVERT(-0.383f, -0.383f), 2.0f, 1.0f,      0.0f, 0.0f, 1.0f,
   THREEDVERT(-0.383f,  0.082f,  1.00f), 1.0f, 1.0f, 1.0f, TWODVERT(-0.383f,  0.082f), 2.0f, 1.0f,      0.0f, 0.0f, 1.0f,
   // 44
   0.432f,  0.082f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(0.432f,  0.082f), 2.0f, 1.0f,                     0.0f, 0.0f, 1.0f,
   0.432f, -0.383f, 1.00f, 1.0f, 1.0f, 1.0f, TWODVERT(0.432f, -0.383f), 2.0f, 1.0f,                     0.0f, 0.0f, 1.0f, 
   THREEDVERT(-0.383f, -0.383f,  1.00f), 1.0f, 1.0f, 1.0f, TWODVERT(-0.383f, -0.383f), 2.0f, 1.0f,      0.0f, 0.0f, 1.0f, 

}; // total verts = 132

// ref: https://www.youtube.com/watch?v=vasfdPx5cvY
float radius = 1.0f;
int subdivision = 12; 
const float angle = (360.0f / float(subdivision));
float originX = 0.0f;
float originY = 0.0f;
float prevX = originX;
float prevY = originY + radius;

float rotateX(float* x, float multiply) {
    *x = radius * sin(glm::radians(angle*multiply));
    return *x;
}

float derotateX(float x, float multiply) { // no pointer so value won't be permanently modified
    x = radius * sin(glm::radians((angle*multiply)-angle));
    return x;
}

float rotateY(float* y, float multiply) { 
    *y = radius * cos(glm::radians(angle*multiply));
    return *y;
}

float derotateY(float y, float multiply) { // no pointer so value won't be permanently modified
    y = radius * cos(glm::radians((angle*multiply)-angle));
    return y;
}

float cake[] = 
{
    //slice cut-off
    
    // originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    // prevX, prevY, 1.0f,                                    1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    // originX, originY, -1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f, 

    
    // originX, originY, -1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    // prevX, prevY, 1.0f,                                    1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    // prevX, prevY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    
    
    //1
    prevX,  prevY,  1.0f,                                  1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    rotateX(&prevX, 1),  rotateY(&prevY, 1),  1.0f,        1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    
    originX, originY,  -1.0f,                              1.0f, 1.0f, 1.0f,   mapTexture(      originX      ), mapTexture(      originY      ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    derotateX(prevX, 1),  derotateY(prevX, 1),  -1.0f,     1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 1)), mapTexture(derotateY(prevY, 1)),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    prevX,  prevY,  -1.0f,                                 1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    derotateX(prevX, 1), derotateY(prevY, 1), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 1)), mapTexture(derotateY(prevY, 1)),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    prevX, prevY,  1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    prevX, prevY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    derotateX(prevX, 1), derotateY(prevY, 1), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 1)), mapTexture(derotateY(prevY, 1)),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    prevX, prevY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    derotateX(prevX, 1), derotateY(prevY, 1), -1.0f,       1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 1)), mapTexture(derotateY(prevY, 1)),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,


    //2
    prevX,  prevY,  1.0f,                                  1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    rotateX(&prevX, 2),  rotateY(&prevY, 2),  1.0f,        1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    originX, originY,  -1.0f,                              1.0f, 1.0f, 1.0f,   mapTexture(      originX      ), mapTexture(      originY      ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    derotateX(prevX, 2),  derotateY(prevX, 2),  -1.0f,     1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 2)), mapTexture(derotateY(prevY, 2)),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    prevX,  prevY,  -1.0f,                                 1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    
    derotateX(prevX, 2), derotateY(prevY, 2), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 2)), mapTexture(derotateY(prevY, 2)),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    prevX, prevY,  1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    prevX, prevY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    derotateX(prevX, 2), derotateY(prevY, 2), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 2)), mapTexture(derotateY(prevY, 2)),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    prevX, prevY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    derotateX(prevX, 2), derotateY(prevY, 2), -1.0f,       1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 2)), mapTexture(derotateY(prevY, 2)),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,


    //3
    prevX,  prevY,  1.0f,                                  1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    rotateX(&prevX, 3),  rotateY(&prevY, 3),  1.0f,        1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    originX, originY,  -1.0f,                              1.0f, 1.0f, 1.0f,   mapTexture(      originX      ), mapTexture(      originY      ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    derotateX(prevX, 3),  derotateY(prevX, 3),  -1.0f,     1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 3)), mapTexture(derotateY(prevY, 3)),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    prevX,  prevY,  -1.0f,                                 1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    derotateX(prevX, 3), derotateY(prevY, 3), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 3)), mapTexture(derotateY(prevY, 3)),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    prevX, prevY,  1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    prevX, prevY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    derotateX(prevX, 3), derotateY(prevY, 3), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 3)), mapTexture(derotateY(prevY, 3)),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    prevX, prevY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    derotateX(prevX, 3), derotateY(prevY, 3), -1.0f,       1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 3)), mapTexture(derotateY(prevY, 3)),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    //4
    prevX,  prevY,  1.0f,                                  1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    rotateX(&prevX, 4),  rotateY(&prevY, 4),  1.0f,        1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    originX, originY,  -1.0f,                              1.0f, 1.0f, 1.0f,   mapTexture(      originX      ), mapTexture(      originY      ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    derotateX(prevX, 4),  derotateY(prevX, 4),  -1.0f,     1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 4)), mapTexture(derotateY(prevY, 4)),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    prevX,  prevY,  -1.0f,                                 1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    derotateX(prevX, 4), derotateY(prevY, 4), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 4)), mapTexture(derotateY(prevY, 4)),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    prevX, prevY,  1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    prevX, prevY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    derotateX(prevX, 4), derotateY(prevY, 4), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 4)), mapTexture(derotateY(prevY, 4)),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    prevX, prevY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    derotateX(prevX, 4), derotateY(prevY, 4), -1.0f,       1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 4)), mapTexture(derotateY(prevY, 4)),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,


    //5
    prevX,  prevY,  1.0f,                                  1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    rotateX(&prevX, 5),  rotateY(&prevY, 5),  1.0f,        1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    originX, originY,  -1.0f,                              1.0f, 1.0f, 1.0f,   mapTexture(      originX      ), mapTexture(      originY      ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    derotateX(prevX, 5),  derotateY(prevX, 5),  -1.0f,     1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 5)), mapTexture(derotateY(prevY, 5)),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    prevX,  prevY,  -1.0f,                                 1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    derotateX(prevX, 5), derotateY(prevY, 5), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 5)), mapTexture(derotateY(prevY, 5)),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    prevX, prevY,  1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    prevX, prevY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    derotateX(prevX, 5), derotateY(prevY, 5), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 5)), mapTexture(derotateY(prevY, 5)),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    prevX, prevY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    derotateX(prevX, 5), derotateY(prevY, 5), -1.0f,       1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 5)), mapTexture(derotateY(prevY, 5)),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    //6
    prevX,  prevY,  1.0f,                                  1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    rotateX(&prevX, 6),  rotateY(&prevY, 6),  1.0f,        1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    originX, originY,  -1.0f,                              1.0f, 1.0f, 1.0f,   mapTexture(      originX      ), mapTexture(      originY      ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    derotateX(prevX, 6),  derotateY(prevX, 6),  -1.0f,     1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 6)), mapTexture(derotateY(prevY, 6)),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    prevX,  prevY,  -1.0f,                                 1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    derotateX(prevX, 6), derotateY(prevY, 6), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 6)), mapTexture(derotateY(prevY, 6)),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    prevX, prevY,  1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    prevX, prevY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    derotateX(prevX, 6), derotateY(prevY, 6), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 6)), mapTexture(derotateY(prevY, 6)),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    prevX, prevY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       prevX       ), mapTexture(       prevY       ),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    derotateX(prevX, 6), derotateY(prevY, 6), -1.0f,       1.0f, 1.0f, 1.0f,   mapTexture(derotateX(prevX, 6)), mapTexture(derotateY(prevY, 6)),   5.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    //slice cut-off
    // prevX, prevY, 1.0f,                                    1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    // originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    // originX, originY, -1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f, 

    // prevX, prevY, 1.0f,                                    1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    // originX, originY, -1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    // prevX, prevY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    // //6
    // prevX,  prevY,  1.0f,                                  1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    // originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    // rotateX(&prevX, 6),  rotateY(&prevY, 6),  1.0f,        1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f,



    // prevX,  prevY,  1.0f,                                  1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    // originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    // rotateX(&prevX, 7),  rotateY(&prevY, 7),  1.0f,        1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    // prevX,  prevY,  1.0f,                                  1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    // originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    // rotateX(&prevX, 8),  rotateY(&prevY, 8),  1.0f,        1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f,
};

float square[] =
{
    -0.80f, -0.80f, 0.00f,   1.0f, 1.0f, 1.0f,   mapTexture( -0.80f ), mapTexture( -0.80f ),   7.0f, 5.0f,   0.0f, 0.0f, 1.0f,
     0.80f, -0.80f, 0.00f,   1.0f, 1.0f, 1.0f,   mapTexture(  0.80f ), mapTexture( -0.80f ),   7.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    -0.80f,  0.80f, 0.00f,   1.0f, 1.0f, 1.0f,   mapTexture( -0.80f ), mapTexture(  0.80f ),   7.0f, 5.0f,   0.0f, 0.0f, 1.0f,

     0.80f,  0.80f, 0.00f,   1.0f, 1.0f, 1.0f,   mapTexture(  0.80f ), mapTexture(  0.80f ),   7.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    -0.80f,  0.80f, 0.00f,   1.0f, 1.0f, 1.0f,   mapTexture( -0.80f ), mapTexture(  0.80f ),   7.0f, 5.0f,   0.0f, 0.0f, 1.0f,
     0.80f, -0.80f, 0.00f,   1.0f, 1.0f, 1.0f,   mapTexture(  0.80f ), mapTexture( -0.80f ),   7.0f, 5.0f,   0.0f, 0.0f, 1.0f,
};

float square2[] =
{
    -0.80f, -0.80f, 0.00f,   1.0f, 1.0f, 1.0f,   mapTexture( -0.80f ), mapTexture( -0.80f ),   8.0f, 5.0f,   0.0f, 0.0f, 1.0f,
     0.80f, -0.80f, 0.00f,   1.0f, 1.0f, 1.0f,   mapTexture(  0.80f ), mapTexture( -0.80f ),   8.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    -0.80f,  0.80f, 0.00f,   1.0f, 1.0f, 1.0f,   mapTexture( -0.80f ), mapTexture(  0.80f ),   8.0f, 5.0f,   0.0f, 0.0f, 1.0f,

     0.80f,  0.80f, 0.00f,   1.0f, 1.0f, 1.0f,   mapTexture(  0.80f ), mapTexture(  0.80f ),   8.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    -0.80f,  0.80f, 0.00f,   1.0f, 1.0f, 1.0f,   mapTexture( -0.80f ), mapTexture(  0.80f ),   8.0f, 5.0f,   0.0f, 0.0f, 1.0f,
     0.80f, -0.80f, 0.00f,   1.0f, 1.0f, 1.0f,   mapTexture(  0.80f ), mapTexture( -0.80f ),   8.0f, 5.0f,   0.0f, 0.0f, 1.0f,
};

// we are going to reuse some values from the cake, but we will also reset some
// reset prevX and prevY back to (0, 1)

float pX = originX;
float pY = originY + radius;

// had to make a new instance of prevX and prevY bc i keep getting a "does not name a type" error OTZ

float cylinder[] = 
{
    //slice cut-off
    
    // originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    // prevX, prevY, 1.0f,                                    1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    // originX, originY, -1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f, 

    
    // originX, originY, -1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    // prevX, prevY, 1.0f,                                    1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    // prevX, prevY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture( prevX ), mapTexture( prevY ),   5.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    
    
    //1
    pX,  pY,  1.0f,                                  1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    rotateX(&pX, 1),  rotateY(&pY, 1),  1.0f,        1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    
    originX, originY,  -1.0f,                              1.0f, 1.0f, 1.0f,   mapTexture(      originX      ), mapTexture(      originY      ),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    derotateX(pX, 1),  derotateY(pX, 1),  -1.0f,     1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 1)), mapTexture(derotateY(pY, 1)),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    pX,  pY,  -1.0f,                                 1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 1), derotateY(pY, 1), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 1)), mapTexture(derotateY(pY, 1)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY,  1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 1), derotateY(pY, 1), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 1)), mapTexture(derotateY(pY, 1)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    derotateX(pX, 1), derotateY(pY, 1), -1.0f,       1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 1)), mapTexture(derotateY(pY, 1)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,


    //2
    pX,  pY,  1.0f,                                  1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    rotateX(&pX, 2),  rotateY(&pY, 2),  1.0f,        1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    originX, originY,  -1.0f,                              1.0f, 1.0f, 1.0f,   mapTexture(      originX      ), mapTexture(      originY      ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    derotateX(pX, 2),  derotateY(pX, 2),  -1.0f,     1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 2)), mapTexture(derotateY(pY, 2)),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    pX,  pY,  -1.0f,                                 1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    
    derotateX(pX, 2), derotateY(pY, 2), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 2)), mapTexture(derotateY(pY, 2)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY,  1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 2), derotateY(pY, 2), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 2)), mapTexture(derotateY(pY, 2)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    derotateX(pX, 2), derotateY(pY, 2), -1.0f,       1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 2)), mapTexture(derotateY(pY, 2)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,


    //3
    pX,  pY,  1.0f,                                  1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    rotateX(&pX, 3),  rotateY(&pY, 3),  1.0f,        1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    originX, originY,  -1.0f,                              1.0f, 1.0f, 1.0f,   mapTexture(      originX      ), mapTexture(      originY      ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    derotateX(pX, 3),  derotateY(pX, 3),  -1.0f,     1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 3)), mapTexture(derotateY(pY, 3)),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    pX,  pY,  -1.0f,                                 1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 3), derotateY(pY, 3), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 3)), mapTexture(derotateY(pY, 3)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY,  1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 3), derotateY(pY, 3), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 3)), mapTexture(derotateY(pY, 3)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    derotateX(pX, 3), derotateY(pY, 3), -1.0f,       1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 3)), mapTexture(derotateY(pY, 3)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    //4
    pX,  pY,  1.0f,                                  1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    rotateX(&pX, 4),  rotateY(&pY, 4),  1.0f,        1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    originX, originY,  -1.0f,                              1.0f, 1.0f, 1.0f,   mapTexture(      originX      ), mapTexture(      originY      ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    derotateX(pX, 4),  derotateY(pX, 4),  -1.0f,     1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 4)), mapTexture(derotateY(pY, 4)),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    pX,  pY,  -1.0f,                                 1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 4), derotateY(pY, 4), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 4)), mapTexture(derotateY(pY, 4)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY,  1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 4), derotateY(pY, 4), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 4)), mapTexture(derotateY(pY, 4)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    derotateX(pX, 4), derotateY(pY, 4), -1.0f,       1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 4)), mapTexture(derotateY(pY, 4)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,


    //5
    pX,  pY,  1.0f,                                  1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    rotateX(&pX, 5),  rotateY(&pY, 5),  1.0f,        1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    originX, originY,  -1.0f,                              1.0f, 1.0f, 1.0f,   mapTexture(      originX      ), mapTexture(      originY      ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    derotateX(pX, 5),  derotateY(pX, 5),  -1.0f,     1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 5)), mapTexture(derotateY(pY, 5)),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    pX,  pY,  -1.0f,                                 1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 5), derotateY(pY, 5), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 5)), mapTexture(derotateY(pY, 5)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY,  1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 5), derotateY(pY, 5), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 5)), mapTexture(derotateY(pY, 5)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    derotateX(pX, 5), derotateY(pY, 5), -1.0f,       1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 5)), mapTexture(derotateY(pY, 5)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,


    //6
    pX,  pY,  1.0f,                                  1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    rotateX(&pX, 6),  rotateY(&pY, 6),  1.0f,        1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    originX, originY,  -1.0f,                              1.0f, 1.0f, 1.0f,   mapTexture(      originX      ), mapTexture(      originY      ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    derotateX(pX, 6),  derotateY(pX, 6),  -1.0f,     1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 6)), mapTexture(derotateY(pY, 6)),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    pX,  pY,  -1.0f,                                 1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 6), derotateY(pY, 6), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 6)), mapTexture(derotateY(pY, 6)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY,  1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 6), derotateY(pY, 6), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 6)), mapTexture(derotateY(pY, 6)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    derotateX(pX, 6), derotateY(pY, 6), -1.0f,       1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 6)), mapTexture(derotateY(pY, 6)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,


    //7
    pX,  pY,  1.0f,                                  1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    rotateX(&pX, 7),  rotateY(&pY, 7),  1.0f,        1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    originX, originY,  -1.0f,                              1.0f, 1.0f, 1.0f,   mapTexture(      originX      ), mapTexture(      originY      ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    derotateX(pX, 7),  derotateY(pX, 7),  -1.0f,     1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 7)), mapTexture(derotateY(pY, 7)),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    pX,  pY,  -1.0f,                                 1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 7), derotateY(pY, 7), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 7)), mapTexture(derotateY(pY, 7)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY,  1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 7), derotateY(pY, 7), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 7)), mapTexture(derotateY(pY, 7)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    derotateX(pX, 7), derotateY(pY, 7), -1.0f,       1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 7)), mapTexture(derotateY(pY, 7)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,


    //8
    pX,  pY,  1.0f,                                  1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    rotateX(&pX, 8),  rotateY(&pY, 8),  1.0f,        1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    originX, originY,  -1.0f,                              1.0f, 1.0f, 1.0f,   mapTexture(      originX      ), mapTexture(      originY      ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    derotateX(pX, 8),  derotateY(pX, 8),  -1.0f,     1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 8)), mapTexture(derotateY(pY, 8)),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    pX,  pY,  -1.0f,                                 1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 8), derotateY(pY, 8), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 8)), mapTexture(derotateY(pY, 8)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY,  1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 8), derotateY(pY, 8), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 8)), mapTexture(derotateY(pY, 8)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    derotateX(pX, 8), derotateY(pY, 8), -1.0f,       1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 8)), mapTexture(derotateY(pY, 8)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,


    //9
    pX,  pY,  1.0f,                                  1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    rotateX(&pX, 9),  rotateY(&pY, 9),  1.0f,        1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    originX, originY,  -1.0f,                              1.0f, 1.0f, 1.0f,   mapTexture(      originX      ), mapTexture(      originY      ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    derotateX(pX, 9),  derotateY(pX, 9),  -1.0f,     1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 9)), mapTexture(derotateY(pY, 9)),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    pX,  pY,  -1.0f,                                 1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 9), derotateY(pY, 9), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 9)), mapTexture(derotateY(pY, 9)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY,  1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 9), derotateY(pY, 9), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 9)), mapTexture(derotateY(pY, 9)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX       ), mapTexture(       pY       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    derotateX(pX, 9), derotateY(pY, 9), -1.0f,       1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 9)), mapTexture(derotateY(pY, 9)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,


    //10
    pX,  pY,  1.0f,                                  1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    rotateX(&pX, 10),  rotateY(&pY, 10),  1.0f,      1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    originX, originY,  -1.0f,                              1.0f, 1.0f, 1.0f,   mapTexture(      originX       ), mapTexture(      originY       ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    derotateX(pX, 10),  derotateY(pX, 10),  -1.0f,   1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 10)), mapTexture(derotateY(pY, 10)),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    pX,  pY,  -1.0f,                                 1.0f, 1.0f, 1.0f,   mapTexture(       pX        ), mapTexture(       pY        ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 10), derotateY(pY, 10), 1.0f,      1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 10)), mapTexture(derotateY(pY, 10)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY,  1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX        ), mapTexture(       pY        ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX        ), mapTexture(       pY        ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 10), derotateY(pY, 10), 1.0f,      1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 10)), mapTexture(derotateY(pY, 10)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX        ), mapTexture(       pY        ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    derotateX(pX, 10), derotateY(pY, 10), -1.0f,     1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 10)), mapTexture(derotateY(pY, 10)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,


    //11
    pX,  pY,  1.0f,                                  1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    rotateX(&pX, 11),  rotateY(&pY, 11),  1.0f,      1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    originX, originY,  -1.0f,                              1.0f, 1.0f, 1.0f,   mapTexture(      originX       ), mapTexture(      originY       ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    derotateX(pX, 11),  derotateY(pX, 11),  -1.0f,   1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 11)), mapTexture(derotateY(pY, 11)),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    pX,  pY,  -1.0f,                                 1.0f, 1.0f, 1.0f,   mapTexture(       pX        ), mapTexture(       pY        ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 11), derotateY(pY, 11), 1.0f,      1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 11)), mapTexture(derotateY(pY, 11)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY,  1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX        ), mapTexture(       pY        ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX        ), mapTexture(       pY        ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 11), derotateY(pY, 11), 1.0f,      1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 11)), mapTexture(derotateY(pY, 11)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX        ), mapTexture(       pY        ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    derotateX(pX, 11), derotateY(pY, 11), -1.0f,     1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 11)), mapTexture(derotateY(pY, 11)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,


    //12
    pX,  pY,  1.0f,                                  1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    originX, originY,  1.0f,                               1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    rotateX(&pX, 12),  rotateY(&pY, 12),  1.0f,      1.0f, 1.0f, 1.0f,   mapTexture( pX ), mapTexture( pY ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    originX, originY,  -1.0f,                              1.0f, 1.0f, 1.0f,   mapTexture(      originX       ), mapTexture(      originY       ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    derotateX(pX, 12),  derotateY(pX, 12),  -1.0f,   1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 12)), mapTexture(derotateY(pY, 12)),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    pX,  pY,  -1.0f,                                 1.0f, 1.0f, 1.0f,   mapTexture(       pX        ), mapTexture(       pY        ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 12), derotateY(pY, 12), 1.0f,      1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 12)), mapTexture(derotateY(pY, 12)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY,  1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX        ), mapTexture(       pY        ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX        ), mapTexture(       pY        ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    derotateX(pX, 12), derotateY(pY, 12), 1.0f,      1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 12)), mapTexture(derotateY(pY, 12)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    pX, pY, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       pX        ), mapTexture(       pY        ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    derotateX(pX, 12), derotateY(pY, 12), -1.0f,     1.0f, 1.0f, 1.0f,   mapTexture(derotateX(pX, 12)), mapTexture(derotateY(pY, 12)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,

};

// commented out; for reference only
// float radius = 1.0f;
// int subdivision = 12; 
// const float angle = (360.0f / float(subdivision)); --> retain
// float originX = 0.0f; --> retain
// float originY = 0.0f; --> retain
// float prevX = originX;
// float prevY = originY + radius;

float r1 = 1.0f; // big circle
float r2 = 0.7f; // small circle
float p1X = originX; // big
float p1Y = originY + r1; // big
float p2X = originX; // small (not much of a difference)
float p2Y = originY + r2; // small

float plate[] = 
{
    //1
    p1X,  p1Y,  1.0f,                                  1.0f, 1.0f, 1.0f,   mapTexture( p1X ), mapTexture( p1Y ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    originX, originY,  1.0f,                           1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    rotateX(&p1X, 1),  rotateY(&p1Y, 1),  1.0f,        1.0f, 1.0f, 1.0f,   mapTexture( p1X ), mapTexture( p1Y ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    
    originX, originY,  -1.0f,                          1.0f, 1.0f, 1.0f,   mapTexture(originX), mapTexture(originY),   4.0f, 5.0f,   0.0f, 0.0f, 1.0f, 
    p2X,  p2Y,  -1.0f,                                 1.0f, 1.0f, 1.0f,   mapTexture(  p2X  ), mapTexture(  p2X  ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,
    rotateX(&p2X, 1),  rotateY(&p2Y, 1),  -1.0f,       1.0f, 1.0f, 1.0f,   mapTexture(  p2X  ), mapTexture(  p2Y  ),   6.0f, 5.0f,   0.0f, 0.0f, 1.0f,

    derotateX(p1X, 1), derotateY(p1Y, 1), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(p1X, 1)), mapTexture(derotateY(p1Y, 1)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    p1X, p1Y,  1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       p1X       ), mapTexture(       p1Y       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    p2X, p2Y, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       p2X       ), mapTexture(       p2Y       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,

    derotateX(p1X, 1), derotateY(p1Y, 1), 1.0f,        1.0f, 1.0f, 1.0f,   mapTexture(derotateX(p1X, 1)), mapTexture(derotateY(p1Y, 1)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    p2X, p2Y, -1.0f,                                   1.0f, 1.0f, 1.0f,   mapTexture(       p2X       ), mapTexture(       p2Y       ),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,
    derotateX(p2X, 1), derotateY(p2Y, 1), -1.0f,       1.0f, 1.0f, 1.0f,   mapTexture(derotateX(p2X, 1)), mapTexture(derotateY(p2Y, 1)),   6.0f, 3.0f,   0.0f, 0.0f, 1.0f,


    //2
    
};


void getNorm(float* verts, int steps, int vertexCount) {
    // int vertexCount = sizeof(verts) / (13 * sizeof(float));
    // int vertexCount = sizeof(verts) / (steps * sizeof(float));
    // cout << "WE ARE ENTERING THE GET NORM FUNCTION" << endl;

    // verts is the list of vertex
    // count is the total elements in verts
    // steps is the strides per row of elements in verts
    // cout << vertexCount << endl;
    for(int i = 0; i < vertexCount; i += 3) {
        glm::vec3 A = glm::vec3(verts[i*steps], verts[i*steps+1], verts[i*steps+2]); 
        // printf("A: (%f, %f, %f)\n", A.x, A.y, A.z);
        glm::vec3 B = glm::vec3(verts[(i+1)*steps], verts[(i+1)*steps+1], verts[(i+1)*steps+2]); 
        // printf("B: (%f, %f, %f)\n", B.x, B.y, B.z);
        glm::vec3 C = glm::vec3(verts[(i+2)*steps], verts[(i+2)*steps+1], verts[(i+2)*steps+2]);
        // printf("C: (%f, %f, %f)\n", C.x, C.y, C.z);

        glm::vec3 U = B - A;
        glm::vec3 V = C - A;
        glm::vec3 normalV = glm::normalize(glm::cross(U,V));

        for(int j = 0; j < 3; j ++) {
            // replace the existing normals with the calculated normals
            verts[(i+j)*steps+10] = normalV.x; 
            verts[(i+j)*steps+11] = normalV.y;
            verts[(i+j)*steps+12] = normalV.z;
            // printf("norms: (%f, %f, %f)\n", verts[(i+j)*steps+10], verts[(i+j)*steps+11], verts[(i+j)*steps+12]);
        }
    }
}

// define OpenGL object IDs to represent the vertex array and the shader program in the GPU
GLuint vao;         // vertex array object (stores the render state for our vertex array) -- bread
GLuint vbo;         // vertex buffer object (reserves GPU memory for our vertex array)

GLuint vao2;        // cake
GLuint vbo2;        // cake

GLuint vao3;        // square
GLuint vbo3;        // square

GLuint vao4;        // candle/cylinder
GLuint vbo4;        // candle/cylinder

GLuint vao5;        // plate
GLuint vbo5;        // plate

GLuint vao6;        // square2
GLuint vbo6;        // square2

GLuint shader;      // combined vertex and fragment shader
GLuint texture;     // texture ID
GLuint texture2;
GLuint cake_txt_inner;
GLuint cake_txt_outer;
GLuint lettuce;

// called by the main function to do initial setup, such as uploading vertex
// arrays, shader programs, etc.; returns true if successful, false otherwise
bool setup()
{
    // FOR VERTEX ARRAY 1 (PUTE LOAF)
    // generate the VAO and VBO objects and store their IDs in vao and vbo, respectively
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    // bind the newly-created VAO to make it the current one that OpenGL will apply state changes to
    glBindVertexArray(vao);

    // calculate for the normals of the verts
    getNorm(vertices, 13, sizeof(vertices) / (13 * sizeof(float)));

    smoothNorm(vertices, 78, 101, 13);
    smoothNorm(vertices, 102, 125, 13);

    // upload our vertex array data to the newly-created VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // on the VAO, register the current VBO with the following vertex attribute layout:
    // - layout location 0...
    // - ... shall consist of 3 GL_FLOATs (corresponding to x, y, and z coordinates)
    // - ... its values will NOT be normalized (GL_FALSE)
    // - ... the stride length is the number of bytes of all 3 floats of each vertex (hence, 3 * sizeof(float))
    // - ... and we start at the beginning of the array (hence, (void*) 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) 0); // position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (3 * sizeof(float))); // color
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (6 * sizeof(float))); // texture
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (8 * sizeof(float))); // texture
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (10 * sizeof(float))); // normal

    // enable the newly-created layout location 0;
    // this shall be used by our vertex shader to read the vertex's x, y, and z
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    // important: if you have more vertex arrays to draw, make sure you separately define them
    // with unique VAO and VBO IDs, and follow the same process above to upload them to the GPU


    // FOR VERTEX ARRAY 2 (cake)
    // generate the VAO and VBO objects and store their IDs in vao and vbo, respectively
    glGenVertexArrays(1, &vao2);
    glGenBuffers(1, &vbo2);

    // bind the newly-created VAO to make it the current one that OpenGL will apply state changes to
    glBindVertexArray(vao2);

    getNorm(cake, 13, sizeof(cake) / (13 * sizeof(float)));
    // smoothNorm(cake, 0, 71, 13);

    // cout << "size: " << sizeof(vertices2)/sizeof(vertices2[0]) << endl;

    // upload our vertex array data to the newly-created VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cake), cake, GL_STATIC_DRAW);

    // on the VAO, register the current VBO with the following vertex attribute layout:
    // - layout location 0...
    // - ... shall consist of 3 GL_FLOATs (corresponding to x, y, and z coordinates)
    // - ... its values will NOT be normalized (GL_FALSE)
    // - ... the stride length is the number of bytes of all 3 floats of each vertex (hence, 3 * sizeof(float))
    // - ... and we start at the beginning of the array (hence, (void*) 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) 0); // position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (3 * sizeof(float))); // color
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (6 * sizeof(float))); // texture
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (8 * sizeof(float))); // texture
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (10 * sizeof(float))); // normal

    // enable the newly-created layout location 0;
    // this shall be used by our vertex shader to read the vertex's x, y, and z
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);


    // FOR VERTEX ARRAY 3 (SQUARE)
    // generate the VAO and VBO objects and store their IDs in vao and vbo, respectively
    glGenVertexArrays(1, &vao3);
    glGenBuffers(1, &vbo3);

    // bind the newly-created VAO to make it the current one that OpenGL will apply state changes to
    glBindVertexArray(vao3);

    getNorm(square, 13, sizeof(square) / (13 * sizeof(float)));

    // cout << "size: " << sizeof(vertices2)/sizeof(vertices2[0]) << endl;

    // upload our vertex array data to the newly-created VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);

    // on the VAO, register the current VBO with the following vertex attribute layout:
    // - layout location 0...
    // - ... shall consist of 3 GL_FLOATs (corresponding to x, y, and z coordinates)
    // - ... its values will NOT be normalized (GL_FALSE)
    // - ... the stride length is the number of bytes of all 3 floats of each vertex (hence, 3 * sizeof(float))
    // - ... and we start at the beginning of the array (hence, (void*) 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) 0); // position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (3 * sizeof(float))); // color
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (6 * sizeof(float))); // texture
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (8 * sizeof(float))); // texture
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (10 * sizeof(float))); // normal

    // enable the newly-created layout location 0;
    // this shall be used by our vertex shader to read the vertex's x, y, and z
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);


    // FOR VERTEX ARRAY 4 (CANDLE/CYLINDER)
    // generate the VAO and VBO objects and store their IDs in vao and vbo, respectively
    glGenVertexArrays(1, &vao4);
    glGenBuffers(1, &vbo4);

    // bind the newly-created VAO to make it the current one that OpenGL will apply state changes to
    glBindVertexArray(vao4);

    getNorm(cylinder, 13, sizeof(cylinder) / (13 * sizeof(float)));
    // smoothNorm(cylinder, 0, 143, 13);

    // upload our vertex array data to the newly-created VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo4);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cylinder), cylinder, GL_STATIC_DRAW);

    // on the VAO, register the current VBO with the following vertex attribute layout:
    // - layout location 0...
    // - ... shall consist of 3 GL_FLOATs (corresponding to x, y, and z coordinates)
    // - ... its values will NOT be normalized (GL_FALSE)
    // - ... the stride length is the number of bytes of all 3 floats of each vertex (hence, 3 * sizeof(float))
    // - ... and we start at the beginning of the array (hence, (void*) 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) 0); // position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (3 * sizeof(float))); // color
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (6 * sizeof(float))); // texture
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (8 * sizeof(float))); // texture
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (10 * sizeof(float))); // normal

    // enable the newly-created layout location 0;
    // this shall be used by our vertex shader to read the vertex's x, y, and z
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);


    // FOR VERTEX ARRAY 5 (PLATE)
    // generate the VAO and VBO objects and store their IDs in vao and vbo, respectively
    glGenVertexArrays(1, &vao5);
    glGenBuffers(1, &vbo5);

    // bind the newly-created VAO to make it the current one that OpenGL will apply state changes to
    glBindVertexArray(vao5);

    getNorm(plate, 13, sizeof(plate) / (13 * sizeof(float)));
    // smoothNorm(cylinder, 0, 143, 13);

    // upload our vertex array data to the newly-created VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo5);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plate), plate, GL_STATIC_DRAW);

    // on the VAO, register the current VBO with the following vertex attribute layout:
    // - layout location 0...
    // - ... shall consist of 3 GL_FLOATs (corresponding to x, y, and z coordinates)
    // - ... its values will NOT be normalized (GL_FALSE)
    // - ... the stride length is the number of bytes of all 3 floats of each vertex (hence, 3 * sizeof(float))
    // - ... and we start at the beginning of the array (hence, (void*) 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) 0); // position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (3 * sizeof(float))); // color
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (6 * sizeof(float))); // texture
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (8 * sizeof(float))); // texture
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (10 * sizeof(float))); // normal

    // enable the newly-created layout location 0;
    // this shall be used by our vertex shader to read the vertex's x, y, and z
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);


    // FOR VERTEX ARRAY 6 (SQUARE 2)
    // generate the VAO and VBO objects and store their IDs in vao and vbo, respectively
    glGenVertexArrays(1, &vao6);
    glGenBuffers(1, &vbo6);

    // bind the newly-created VAO to make it the current one that OpenGL will apply state changes to
    glBindVertexArray(vao6);

    getNorm(square2, 13, sizeof(square2) / (13 * sizeof(float)));
    // smoothNorm(cylinder, 0, 143, 13);

    // upload our vertex array data to the newly-created VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo6);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square2), square2, GL_STATIC_DRAW);

    // on the VAO, register the current VBO with the following vertex attribute layout:
    // - layout location 0...
    // - ... shall consist of 3 GL_FLOATs (corresponding to x, y, and z coordinates)
    // - ... its values will NOT be normalized (GL_FALSE)
    // - ... the stride length is the number of bytes of all 3 floats of each vertex (hence, 3 * sizeof(float))
    // - ... and we start at the beginning of the array (hence, (void*) 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) 0); // position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (3 * sizeof(float))); // color
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (6 * sizeof(float))); // texture
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (8 * sizeof(float))); // texture
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*) (10 * sizeof(float))); // normal

    // enable the newly-created layout location 0;
    // this shall be used by our vertex shader to read the vertex's x, y, and z
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);


    // load our shader program
    shader = gdevLoadShader("finals.vs", "finals.fs");
    if (! shader)
        return false;

    texture = gdevLoadTexture("pipipi.png", GL_REPEAT, true, true);
    texture2 = gdevLoadTexture("crust.png", GL_REPEAT, true, true);
    cake_txt_inner = gdevLoadTexture("cake-inner.png", GL_REPEAT, true, true);
    cake_txt_outer = gdevLoadTexture("cake-outer.png", GL_REPEAT, true, true);
    lettuce = gdevLoadTexture("lettuce.png", GL_REPEAT, true, true);

    if (!texture) return false;
    if (!texture2) return false;
    if (!cake_txt_inner) return false;
    if (!cake_txt_outer) return false;
    if (!lettuce) return false;

    //gdevLoadTexture is defined in gdev.h, and its parameters are:
    // - the texture filename
    // - the wrap mode (GL_REPEAT, GL_MIRRORED_REPEAT, ...)
    // - whether to enable linear filtering

    return true;
}

// called by the main function to do rendering per frame
void render()
{

    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // clear the whole frame
    glClearColor(0.08f, 0.15f, 0.27f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float t = glfwGetTime(); 
    glUniform1f(glGetUniformLocation(shader, "t"), t);
    glUniform3f(glGetUniformLocation(shader, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
    glUniform3f(glGetUniformLocation(shader, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
    // glUniform3f(glGetUniformLocation(shader, "lightColor"), 1.0f, 0.95f, 0.76f);
    glUniform1f(glGetUniformLocation(shader, "specularity"), specularity);

    // using our shader program...
    glUseProgram(shader);

    glEnable(GL_DEPTH_TEST); // enable OpenGL's hidden surface removal

    // set 3D transformations
    // glm::mat4 matrix;
    // matrix = glm::perspective(glm::radians(30.0f), (float) WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    // matrix = glm::translate(matrix, glm::vec3(0.0f, 0.0f, -5.0f));
    // matrix = glm::rotate(matrix, glm::radians(abs(t) * 200), glm::vec3(1.0f, 1.0f, 1.0f)); // you spin me right round baby...
    // // matrix = glm::rotate(matrix, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // stagnant rotation
    // matrix = glm::scale(matrix, glm::vec3(1.0f, 1.0f, 1.0f));
    // glUniformMatrix4fv(glGetUniformLocation(shader, "matrix"), 1, GL_FALSE, glm::value_ptr(matrix));

    // camera stuff
    glm::mat4 projectionViewMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    projectionViewMatrix = glm::perspective(glm::radians(60.0f), (float) WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);    
    projectionViewMatrix *= glm::lookAt(cameraPos, cameraFront, cameraUp);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projectionViewMatrix"), 1, GL_FALSE, glm::value_ptr(projectionViewMatrix)); // call after every draw arrays
    
    // ... set the active texture...
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, cake_txt_inner);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, cake_txt_outer);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, lettuce);

    glUniform1i(glGetUniformLocation(shader, "shaderTextureA"), 0);
    glUniform1i(glGetUniformLocation(shader, "shaderTextureB"), 1);
    glUniform1i(glGetUniformLocation(shader, "cake_txt_inner"), 2);
    glUniform1i(glGetUniformLocation(shader, "cake_txt_outer"), 3);
    glUniform1i(glGetUniformLocation(shader, "lettuce_txt"), 4);

    // ... draw our triangles
    glBindVertexArray(vao);    
    
    // draw bread
    // bottom bread
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -0.42f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // you spin me right round baby..
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0, 1.0f, 0.05f));
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix)); // call after every draw arrays

    // normal matrix
    glm::mat4 normalMatrix;
    normalMatrix = glm::transpose(glm::inverse(modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix)); // call after every draw arrays
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / (6 * sizeof(float)));


    // top bread
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.2f, -0.30f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // you spin me right round baby..
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0, 1.0f, 0.05f));
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix)); // call after every draw arrays

    // normal matrix
    normalMatrix = glm::transpose(glm::inverse(modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix)); // call after every draw arrays
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / (6 * sizeof(float)));
    

    // draw lettuce
    glBindVertexArray(vao3);
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, -0.35f));
    // modelMatrix = glm::rotate(modelMatrix, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
    // modelMatrix = glm::rotate(modelMatrix, glm::radians(abs(t)*50), glm::vec3(0.0f, 1.0f, 0.0f)); // for debugging
    // modelMatrix = glm::scale(modelMatrix, glm::vec3(1.2f, 1.2f, 1.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix)); // call after every draw arrays

    // normal matrix
    normalMatrix = glm::transpose(glm::inverse(modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix)); // call after every draw arrays

    glDrawArrays(GL_TRIANGLES, 0, sizeof(square) / (6 * sizeof(float)));


    // draw cake
    glBindVertexArray(vao2);
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // rotate cake sideways so the flat part is facing the "ground"
    modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.45f, 2.0f, 0.0f)); // move cake to the loaf's right side
    modelMatrix = glm::rotate(modelMatrix, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
    // modelMatrix = glm::rotate(modelMatrix, glm::radians(abs(t)*50), glm::vec3(0.0f, 1.0f, 0.0f)); // for debugging
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix)); // call after every draw arrays

    // normal matrix
    normalMatrix = glm::transpose(glm::inverse(modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix)); // call after every draw arrays

    glDrawArrays(GL_TRIANGLES, 0, sizeof(cake) / (6 * sizeof(float)));


    // draw candle (base)
    glBindVertexArray(vao4);
    modelMatrix = glm::mat4(1.0f);
    // modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // rotate cake sideways so the flat part is facing the "ground"
    modelMatrix = glm::translate(modelMatrix, glm::vec3(1.0f, 0.0f, 0.0f)); // move candle to the bread's right side
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
    // modelMatrix = glm::rotate(modelMatrix, glm::radians(abs(t)*50), glm::vec3(1.0f, 0.0f, 0.0f)); // for debugging
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2, 0.2f, 0.5f));
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix)); // call after every draw arrays

    // normal matrix
    normalMatrix = glm::transpose(glm::inverse(modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix)); // call after every draw arrays

    glDrawArrays(GL_TRIANGLES, 0, sizeof(cylinder) / (6 * sizeof(float)));


    // draw candle (tiny thingy)
    modelMatrix = glm::mat4(1.0f);
    // modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // rotate cake sideways so the flat part is facing the "ground"
    modelMatrix = glm::translate(modelMatrix, glm::vec3(1.0f, 0.50f, 0.0f)); // move candle to the bread's right side
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
    // modelMatrix = glm::rotate(modelMatrix, glm::radians(abs(t)*50), glm::vec3(1.0f, 0.0f, 0.0f)); // for debugging
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01, 0.01f, 0.1f));
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix)); // call after every draw arrays

    // normal matrix
    normalMatrix = glm::transpose(glm::inverse(modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix)); // call after every draw arrays

    glDrawArrays(GL_TRIANGLES, 0, sizeof(cylinder) / (6 * sizeof(float)));


    // draw light 
    glBindVertexArray(vao6);
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(1.0f, 0.80f, 0.0f)); // move candle to the bread's right side
    modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)); 
    // modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
    // modelMatrix = glm::rotate(modelMatrix, glm::radians(abs(t)*50), glm::vec3(1.0f, 0.0f, 0.0f)); // for debugging
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.15f, 1.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix)); // call after every draw arrays

    // normal matrix
    normalMatrix = glm::transpose(glm::inverse(modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix)); // call after every draw arrays

    glDrawArrays(GL_TRIANGLES, 0, sizeof(square2) / (6 * sizeof(float)));

        
    // // draw plate
    // glBindVertexArray(vao5);
    // modelMatrix = glm::mat4(1.0f);
    // modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 2.0f, 0.0f)); // move cake to the loaf's right side
    // // modelMatrix = glm::rotate(modelMatrix, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
    // // modelMatrix = glm::rotate(modelMatrix, glm::radians(abs(t)*50), glm::vec3(0.0f, 1.0f, 0.0f)); // for debugging
    // glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix)); // call after every draw arrays

    // // normal matrix
    // normalMatrix = glm::transpose(glm::inverse(modelMatrix));
    // glUniformMatrix4fv(glGetUniformLocation(shader, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix)); // call after every draw arrays

    // glDrawArrays(GL_TRIANGLES, 0, sizeof(plate) / (6 * sizeof(float)));


    // // second (left)
    // modelMatrix = glm::rotate(modelMatrix, glm::radians(abs(t) * 50), glm::vec3(-1.0f, 0.0f, 0.0f)); // revert rotate
    // modelMatrix = glm::translate(modelMatrix, glm::vec3(-3.0f, 0.0f, 0.0f)); // revert translate origin
    // modelMatrix = glm::rotate(modelMatrix, glm::radians(abs(t) * 50), glm::vec3(0.0f, 1.0f, 0.0f));
    // modelMatrix = glm::scale(modelMatrix, glm::vec3(1.5f, 2.5f, 1.5f));
    // glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix)); // call after every draw arrays
    // // normal matrix
    // normalMatrix = glm::transpose(glm::inverse(modelMatrix));
    // glUniformMatrix4fv(glGetUniformLocation(shader, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix)); // call after every draw arrays

    // glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / (6 * sizeof(float)));


    // // third (right)
    // modelMatrix = glm::rotate(modelMatrix, glm::radians(-abs(t) * 50), glm::vec3(0.0f, 1.0f, .0f)); // revert rotate
    // modelMatrix = glm::translate(modelMatrix, glm::vec3(3.0f, 0.0f, 0.0f)); // revert translate back to origin
    // modelMatrix = glm::translate(modelMatrix, glm::vec3(0.8f, 0.0f, 0.0f));
    // modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f, 0.5f, 0.2f));
    // modelMatrix = glm::rotate(modelMatrix, glm::radians(abs(t) * 50), glm::vec3(0.0f, 0.0f, 1.0f));
    // glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix)); // call after every draw arrays
    
    // // normal matrix
    // normalMatrix = glm::transpose(glm::inverse(modelMatrix));
    // glUniformMatrix4fv(glGetUniformLocation(shader, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix)); // call after every draw arrays

    // glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / (6 * sizeof(float)));

    // CULL clockwise
    glEnable(GL_CULL_FACE);
}

/*****************************************************************************/

// handler called by GLFW when there is a keyboard event
void handleKeys(GLFWwindow* pWindow, int key, int scancode, int action, int mode)
{
    // pressing Esc closes the window
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(pWindow, GL_TRUE);

    const float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
    if (glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    // lighting controls
    const float lightSpeed = 300.0f * deltaTime;
    if (glfwGetKey(pWindow, GLFW_KEY_UP) == GLFW_PRESS)
        lightPosition.y += lightSpeed;
    if (glfwGetKey(pWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
        lightPosition.y -= lightSpeed;
    if (glfwGetKey(pWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
        lightPosition.x -= lightSpeed;
    if (glfwGetKey(pWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
        lightPosition.x += lightSpeed;
    if (glfwGetKey(pWindow, GLFW_KEY_Z) == GLFW_PRESS)
        lightPosition.z -= lightSpeed;
    if (glfwGetKey(pWindow, GLFW_KEY_X) == GLFW_PRESS)
        lightPosition.z += lightSpeed;

    // specularity controls
    float limit = 31.0f;
    const float specularChange = 10000.0f * deltaTime;
    if (glfwGetKey(pWindow, GLFW_KEY_Q) == GLFW_PRESS) {
        if (specularity > limit)
            specularity -= specularChange;
        else
            specularity = limit;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_E) == GLFW_PRESS)
        specularity += specularChange;
    
    printf("light pos: (%f, %f, %f)\n", lightPosition.x, lightPosition.y, lightPosition.z);
    // cout << "specularity: " << specularity << endl;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
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

    glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  

    // make the window the current context of subsequent OpenGL commands,
    // and enable vertical sync and aspect-ratio correction on the GLFW window
    glfwMakeContextCurrent(pWindow);
    glfwSwapInterval(1);
    glfwSetWindowAspectRatio(pWindow, WINDOW_WIDTH, WINDOW_HEIGHT);

    // set up callback functions to handle window system events
    glfwSetKeyCallback(pWindow, handleKeys);
    glfwSetCursorPosCallback(pWindow, mouse_callback);
    glfwSetScrollCallback(pWindow, scroll_callback);
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

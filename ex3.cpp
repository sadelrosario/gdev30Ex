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
GLFWwindow *pWindow;

float mapTexture(float coordinate) {
    return (coordinate / 2) + 0.5f;
}

// define a vertex array to hold our vertices
float vertices[] =
{
    // position (x, y, z)  color (r, g, b)   texture coordinates (s, t)
    
    // base
    -0.383f, 0.082f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.383f), mapTexture(0.082f),
    0.432f, 0.082f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(0.432f), mapTexture(0.082f),
    0.024f, 0.303f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(0.024f), mapTexture(0.303f),

    -0.383f, 0.082f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.383f), mapTexture(0.082f),
    -0.383f, -0.383f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.383f), mapTexture(-0.383f),
    0.432f, 0.082f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(0.432f), mapTexture(0.082f),

    -0.383f, -0.383f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.383f), mapTexture(-0.383f),
    0.432f, -0.383f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(0.432f), mapTexture(-0.383f),
    0.432f, 0.082f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(0.432f), mapTexture(0.082f),

    // left ear
    -0.462f, 0.253f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.462f), mapTexture(0.253f),
    -0.379f, 0.079f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.379f), mapTexture(0.079f),
    -0.351f, 0.439f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.351f), mapTexture(0.439f),

    -0.351f, 0.439f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.351f), mapTexture(0.439f),
    -0.381f, 0.078f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.381f), mapTexture(0.078f),
    -0.158f, 0.167f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.158f), mapTexture(0.167f),

    -0.351f, 0.439f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.351f), mapTexture(0.439f),
    -0.158f, 0.167f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.158f), mapTexture(0.167f),
    -0.131f, 0.450f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.131f), mapTexture(0.450f),

    -0.131f, 0.450f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.131f), mapTexture(0.450f),
    -0.158f, 0.167f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(-0.158f), mapTexture(0.167f),
    0.032f, 0.291f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(0.032f), mapTexture(0.291f),

    // right ear
    0.396f, 0.439f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(0.396f), mapTexture(0.439f),
    0.425f, 0.082f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(0.425f), mapTexture(0.082f),
    0.505f, 0.253f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(0.505f), mapTexture(0.253f),

    0.200f, 0.166f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(0.200f), mapTexture(0.166),
    0.425f, 0.077f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(0.425f), mapTexture(0.077f),
    0.396f, 0.439f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(0.396f), mapTexture(0.439f),

    0.176f, 0.450f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(0.176f), mapTexture(0.450f),
    0.200f, 0.166f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(0.200f), mapTexture(0.166),
    0.396f, 0.439f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(0.396f), mapTexture(0.439f),

    0.176f, 0.450f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(0.176f), mapTexture(0.450f),
    0.011f, 0.290f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(0.011f), mapTexture(0.290f),
    0.207f, 0.163f, -1.00f, 1.0f, 1.0f, 1.0f, mapTexture(0.207), mapTexture(0.163f),
};

// define OpenGL object IDs to represent the vertex array and the shader program in the GPU
GLuint vao;         // vertex array object (stores the render state for our vertex array)
GLuint vbo;         // vertex buffer object (reserves GPU memory for our vertex array)
GLuint shader;      // combined vertex and fragment shader
GLuint texture;     // texture ID
GLuint texture2;

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
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float))); // texture

    // enable the newly-created layout location 0;
    // this shall be used by our vertex shader to read the vertex's x, y, and z
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // important: if you have more vertex arrays to draw, make sure you separately define them
    // with unique VAO and VBO IDs, and follow the same process above to upload them to the GPU

    // load our shader program
    shader = gdevLoadShader("ex3.vs", "ex3.fs");
    if (! shader)
        return false;

    texture = gdevLoadTexture("pipipi.png", GL_REPEAT, true, true);
    texture2 = gdevLoadTexture("childe.jpg", GL_REPEAT, true, true);

    if (!texture) return false;
    if (!texture2) return false;

    //gdevLoadTexture is defined in gdev.h, and its parameters are:
    // - the texture filename
    // - the wrap mode (GL_REPEAT, GL_MIRRORED_REPEAT, ...)
    // - whether to enable linear filtering

    return true;
}

// called by the main function to do rendering per frame
void render()
{
    // clear the whole frame
    glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float t = glfwGetTime(); 
    glUniform1f(glGetUniformLocation(shader, "t"), t);

    // using our shader program...
    glUseProgram(shader);

    // ... set the active texture...
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glUniform1i(glGetUniformLocation(shader, "shaderTextureA"), 0);
    glUniform1i(glGetUniformLocation(shader, "shaderTextureB"), 1);

    // ... draw our triangles
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / (6 * sizeof(float)));
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

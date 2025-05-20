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
#include<string>
#include <vector>
using namespace std;

// #include "mesh.h"

// change this to your desired window attributes
#define WINDOW_WIDTH  1000
#define WINDOW_HEIGHT 1000
#define WINDOW_TITLE  "Hello Flag (Sherrie Del Rosario)"
GLFWwindow *pWindow;
GLuint texture;
GLuint texture2;
GLuint colorBuffers[2];

float mapTexture(float coordinate) {
    return (coordinate / 2) + 0.5f;
}


// define a vertex array to hold our vertices
float vertices[] =
{
    // green
     1.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f, mapTexture( 1.0f ), mapTexture( 0.0f), 2.0f, 1.0f,
    -1.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f, mapTexture(-1.0f),  mapTexture(-1.0f), 2.0f, 1.0f,
     1.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f, mapTexture( 1.0f),  mapTexture(-1.0f), 2.0f, 1.0f,


    // red
    - 1.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f, mapTexture(-1.0f ), mapTexture( 0.0f), 1.0f, 1.0f, 
    - 1.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f, mapTexture(-1.0f ), mapTexture(-1.0f), 1.0f, 1.0f, 
     1.0f,   0.0f, 0.0f,  1.0f, 0.0f, 0.0f, mapTexture( 1.0f ), mapTexture( 0.0f), 1.0f, 1.0f,

    // // blue
    // - 1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f, mapTexture(- 1.0f), mapTexture(1.0f),
    // - 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f, mapTexture(- 1.0f), mapTexture(0.0f),
    // 1.0f,   1.0f,  0.0f, 0.0f, 0.0f, 1.0f, mapTexture(1.0f), mapTexture(1.0f),
    
    // - 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f,
    // 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f, 
    // 1.0f,   1.0f,  0.0f, 0.0f, 0.0f, 1.0f,


    // small white
    // - 1.0f,  1.0f,  0.0f,  1.0f, 1.0f, 1.0f,
    // - 0.25f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f,
    // - 1.0f,  -1.0f,  0.0f,  1.0f, 1.0f, 1.0f,

    // // position (x, y, z) color (r, g, b) texture coordinates (s, t)
    //  -0.30f, -0.50f, 0.00f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    //  0.30f, -0.50f, 0.00f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    //  -0.30f, 0.50f, 0.00f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
     
    //  0.40f, 0.80f, 0.20f, 1.0f, 1.0f, 0.0f, 2.0f, 2.0f,
    //  -0.40f, 0.80f, 0.20f, 0.1f, 1.0f, 1.0f, 0.0f, 2.0f,
    //  0.40f, -0.80f, 0.20f, 1.0f, 0.0f, 1.0f, 2.0f, 0.0f

    // -0.0929f, 0.7203f, 0.0f,  1.0f, 1.0f, 1.0f, 0.4521f, 0.8622f,
    // -0.7189f, 0.3219f, 0.0f,  1.0f, 1.0f, 1.0f, 0.1391f, 0.6591f,
    // -0.2415f, 0.1575f, 0.0f,  1.0f, 1.0f, 1.0f, 0.3803f, 0.5809f,

   
};

float screenVertices[] =
{
	// Coords    // texCoords
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,

	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f
};

// define OpenGL object IDs to represent the vertex array and the shader program in the GPU
GLuint vao;         // vertex array object (stores the render state for our vertex array)
GLuint vbo;         // vertex buffer object (reserves GPU memory for our vertex array)
GLuint shader;      // combined vertex and fragment shader
GLuint particleShader;
GLuint program;
GLuint post;
// framebuffers ID
GLuint pingpongFBO[2];
GLuint pingpongBuffer[2];
GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
GLuint fbo;
GLuint framebufferTexture;
GLuint rbo;
GLuint screenVBO, screenVAO;
GLuint fbShader;
GLuint stencilShader;
GLuint bloomTexture;

void drawScene() 
{
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / (10 * sizeof(float))); // draw vertices
}

// called by the main function to do initial setup, such as uploading vertex
// arrays, shader programs, etc.; returns true if successful, false otherwise
bool setup()
{


    // // screen
	glGenVertexArrays(1, &screenVAO);
	glGenBuffers(1, &screenVBO);
	glBindVertexArray(screenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), &screenVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // load textures 
    texture = gdevLoadTexture("pepe.png", GL_REPEAT, true, true);
    if (! texture) return false;
    texture2 = gdevLoadTexture("crust.jpg", GL_REPEAT, true, true);
    if (! texture2) return false;

    
    // // frame buffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // textures
    glGenTextures(1, &framebufferTexture);
    glBindTexture(GL_TEXTURE_2D, framebufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);


    
    // Create Second Framebuffer Texture to duplicate the glowing colors with the blur
	glGenTextures(1, &bloomTexture);
	glBindTexture(GL_TEXTURE_2D, bloomTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bloomTexture, 0);

    // draw both buffers
    glDrawBuffers(2, attachments);

    // render
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // BLUR framebuffer PINGPONG
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongBuffer);
    for(int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0
        );
        auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Ping-Pong Framebuffer error: " << fboStatus << std::endl;
    }

    // error handling
    auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(fboStatus != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "framebuffer error: " << fboStatus << std::endl;

    // // shader for framebuffers
    fbShader = gdevLoadShader("fb.vs", "fb.fs");
    if (! fbShader)
        return false;

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*) 0);                     // x y z coordinates
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*) (3 * sizeof(float)));   // r g b colors
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*) (6 * sizeof(float)));   // texture coordinates
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*) (8 * sizeof(float)));   // indicator


    // enable the newly-created layout location 0;
    // this shall be used by our vertex shader to read the vertex's x, y, and z
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    // important: if you have more vertex arrays to draw, make sure you separately define them
    // with unique VAO and VBO IDs, and follow the same process above to upload them to the GPU

    // particleShader = gdevLoadShader("particle.vs", "particle.fs");
    // if (! shader)
    //     return false;

    program = gdevLoadShader("fb.vs", "blur.fs");
    if (! program)
        return false;
    shader = gdevLoadShader("demo1.vs", "demo1.fs");
    if (! shader)
        return false;



    // // blur fragment shader
    // const char* cstr;
    // std::string blurShader;
    // blurShader = gdevLoadFile("blur.fs");
    // cstr = blurShader.data();
    // GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // create shader
    // glShaderSource(fragmentShader, 1, &cstr, NULL); // compile shader
    // glCompileShader(fragmentShader);
    // if (! fragmentShader) {
    //     glDeleteShader(fragmentShader); 
    //     return 0;
    // }
    // program = glCreateProgram(); 
    // glAttachShader(program, fragmentShader);
    // glLinkProgram(program);
    // // glDeleteShader(fragmentShader); // delete shader after linked to program
    // int success, length;
    // glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    // if (! success)
    // {
    //     glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &length);
    //     std::string err(length, ' ');
    //     glGetShaderInfoLog(fragmentShader, length, NULL, err.data());
    //     std::cout << "Fragment shader file 'blur shader' compile error:\n" << err;
    //     glDeleteShader(fragmentShader);
    //     glDeleteProgram(program);
    //     return 0;
    // }
    // glGetProgramiv(program, GL_LINK_STATUS, &success);
    // if (! success)
    // {
    //     glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    //     std::string err(length, ' ');
    //     glGetProgramInfoLog(program, length, NULL, err.data());
    //     std::cout << "Shader program link error:\n" << err;
    //     glDeleteShader(fragmentShader);
    //     glDeleteProgram(program);
    //     return 0;
    // }

    // // BLOOM color framebuffers (2 buffers)         
    // glGenTextures(1, colorBuffers);
    // for(int i = 0; i < 2; i++)
    // {
    //     glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    // }


    // gdevLoadTexture() is defined in gdev.h, and its parameters are 
    // - the texture filename
    // the wrap mode

    return true;
}

// called by the main function to do rendering per frame
void render()
{

    // using our shader program...
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);  
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // bg color WHITE
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);


    glUseProgram(shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shader, "shaderTextureA"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glUniform1i(glGetUniformLocation(shader, "shaderTextureB"), 1);

    glUseProgram(fbShader);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, framebufferTexture);
    glUniform1i(glGetUniformLocation(fbShader, "screenTexture"), 7);
    glUniform1i(glGetUniformLocation(program, "screenTexture"), 7);

    drawScene();

    // BLUR
    bool horizontal = true;
    bool first_iteration = true;
    
    int amount = 2; // minimum of 2
    // use extra shader for blur...
    glUseProgram(program);
    for(int i = 0; i < amount; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        glUniform1f(glGetUniformLocation(program, "horizontal"), horizontal);

        if(first_iteration)
        {
            glBindTexture(GL_TEXTURE_2D, bloomTexture);
            first_iteration = false;
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal]);
        }

        // render image
        glBindVertexArray(screenVAO);
        glDisable(GL_DEPTH_TEST);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // switch between horizontal and vertical blurring
        horizontal = !horizontal;
    }

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal]);
    glUniform1i(glGetUniformLocation(fbShader, "bloomTexture"), 3);




    // bind default framebuffer

    // glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal]);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(fbShader); // use the post processing shader (after using all shaders)
    glBindVertexArray(screenVAO); // bind screen rectangle vertex array
    glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, framebufferTexture);
    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal]);
    // order of active does  matter ????

    

    // how to put this texture inside framebufferTexture ?????
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, texture);
    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, texture2);




    glDrawArrays(GL_TRIANGLES, 0, 6);


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

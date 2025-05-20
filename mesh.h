#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <gdev.h>
#include<string>
#include <vector>
using namespace std;
std::string get_file_contents(const char* filename);

class Shader // so we can call to draw the shader
    {
        public:
            // Reference ID of the Shader Program
            GLuint ID;
            // Constructor that build the Shader Program from 2 different shaders
            Shader(const char* vertexFile, const char* fragmentFile);
            Shader(const char* vertexFile, const char* fragmentFile, const char* geometryFile);

            // Activates the Shader Program
            void Activate();
            // Deletes the Shader Program
            void Delete();
        private:
            // Checks if the different Shaders have compiled properly
            void compileErrors(unsigned int shader, const char* type);
    };


struct Vertex 
    {
        glm::vec3 position;
        glm::vec3 colors;
        glm::vec2 texCoords;
        glm::vec2 indicators;
    };

struct Texture 
    {
        GLuint id;
        std::string type;
    };

class Mesh
    {
    public:
        std::vector <Vertex> vertices;
        std::vector <GLuint> indices;
        std::vector <Texture> textures;
        GLuint vbo, vao, ebo;
        // Initializes the mesh
        Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures);

        // Draws the mesh
        void Draw
        (
            Shader& shader, 
            glm::mat4 matrix = glm::mat4(1.0f),
            glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
            glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
            glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)
        );
    };

#endif
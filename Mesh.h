#ifndef Mesh_h
#define Mesh_h

#include <GL/glew.h>
#include <GLFW/glfw3.h> // The GLFW header
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


/*
    Using the same structs as used in sampleGL file. These are suitable for the parser given
 
    Layout location in shaders are as the following:
    
    location 0: vec3 pos
    location 1: vec2 uv
    location 2: vec3 normal
 
 
    As of buffer layout itself:
    I generally prefer interleaving the data of each vertex in the buffer. But, when loading vertex data from
    file we generally retrieve an array of positions, an array of normals, an array of textures etc. This is also
    the case in this homework as we load data from obj file. It will cost and effort to interleave the data. Instead,
    I will take an batched approach
 
*/
struct Vertex
{
    Vertex(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
    GLfloat x, y, z;
};

struct Texture
{
    Texture(GLfloat inU, GLfloat inV) : u(inU), v(inV) { }
    GLfloat u, v;
};

struct Normal
{
    Normal(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
    GLfloat x, y, z;
};

struct Face
{
    Face(int v[], int t[], int n[]) {
        vIndex[0] = v[0];
        vIndex[1] = v[1];
        vIndex[2] = v[2];
        tIndex[0] = t[0];
        tIndex[1] = t[1];
        tIndex[2] = t[2];
        nIndex[0] = n[0];
        nIndex[1] = n[1];
        nIndex[2] = n[2];
    }
    GLuint vIndex[3], tIndex[3], nIndex[3];
};


class Mesh
{
public:
    Mesh();
    Mesh(const std::string& fileName);
    
    GLuint getVAO() const;
    int getNumTriangles() const;
    
private:
    GLuint VAO, VBO, EBO;
    int numTriangles; //This is needed for draw calls
};


#endif /* Mesh_h */

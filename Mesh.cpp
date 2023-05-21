#include "Mesh.h"

//Parser
bool ParseObj
(
 const std::string& fileName,
 std::vector<Vertex>& gVertices,
 std::vector<Texture>& gTextures,
 std::vector<Normal>& gNormals,
 std::vector<Face>& gFaces
)

{
    using namespace std;
    
    fstream myfile;

    // Open the input
    myfile.open(fileName.c_str(), std::ios::in);

    if (myfile.is_open())
    {
        string curLine;

        while (getline(myfile, curLine))
        {
            stringstream str(curLine);
            GLfloat c1, c2, c3;
            GLuint index[9];
            string tmp;

            if (curLine.length() >= 2)
            {
                if (curLine[0] == 'v')
                {
                    if (curLine[1] == 't') // texture
                    {
                        str >> tmp; // consume "vt"
                        str >> c1 >> c2;
                        gTextures.push_back(Texture(c1, c2));
                    }
                    else if (curLine[1] == 'n') // normal
                    {
                        str >> tmp; // consume "vn"
                        str >> c1 >> c2 >> c3;
                        gNormals.push_back(Normal(c1, c2, c3));
                    }
                    else // vertex
                    {
                        str >> tmp; // consume "v"
                        str >> c1 >> c2 >> c3;
                        gVertices.push_back(Vertex(c1, c2, c3));
                    }
                }
                else if (curLine[0] == 'f') // face
                {
                    str >> tmp; // consume "f"
                    char c;
                    int vIndex[3], nIndex[3], tIndex[3];
                    str >> vIndex[0]; str >> c >> c; // consume "//"
                    str >> nIndex[0];
                    str >> vIndex[1]; str >> c >> c; // consume "//"
                    str >> nIndex[1];
                    str >> vIndex[2]; str >> c >> c; // consume "//"
                    str >> nIndex[2];

                    assert(vIndex[0] == nIndex[0] &&
                        vIndex[1] == nIndex[1] &&
                        vIndex[2] == nIndex[2]); // a limitation for now

                    // make indices start from 0
                    for (int c = 0; c < 3; ++c)
                    {
                        vIndex[c] -= 1;
                        nIndex[c] -= 1;
                        tIndex[c] -= 1;
                    }

                    gFaces.push_back(Face(vIndex, tIndex, nIndex));
                }
                else
                {
                    cout << "Ignoring unidentified line in obj file: " << curLine << endl;
                }
            }

            //data += curLine;
            if (!myfile.eof())
            {
                //data += "\n";
            }
        }

        myfile.close();
    }
    else
    {
        return false;
    }

    /*
    for (int i = 0; i < gVertices.size(); ++i)
    {
        Vector3 n;

        for (int j = 0; j < gFaces.size(); ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                if (gFaces[j].vIndex[k] == i)
                {
                    // face j contains vertex i
                    Vector3 a(gVertices[gFaces[j].vIndex[0]].x,
                              gVertices[gFaces[j].vIndex[0]].y,
                              gVertices[gFaces[j].vIndex[0]].z);

                    Vector3 b(gVertices[gFaces[j].vIndex[1]].x,
                              gVertices[gFaces[j].vIndex[1]].y,
                              gVertices[gFaces[j].vIndex[1]].z);

                    Vector3 c(gVertices[gFaces[j].vIndex[2]].x,
                              gVertices[gFaces[j].vIndex[2]].y,
                              gVertices[gFaces[j].vIndex[2]].z);

                    Vector3 ab = b - a;
                    Vector3 ac = c - a;
                    Vector3 normalFromThisFace = (ab.cross(ac)).getNormalized();
                    n += normalFromThisFace;
                }

            }
        }

        n.normalize();

        gNormals.push_back(Normal(n.x, n.y, n.z));
    }
    */

    assert(gVertices.size() == gNormals.size());

    return true;
}

Mesh::Mesh()
 {}


Mesh::Mesh(const std::string& fileName)
{
    //Create the temporary buffers which will store the data loaded from the file
    std::vector<Vertex> gVertices;
    std::vector<Texture> gTextures;
    std::vector<Normal> gNormals;
    std::vector<Face> gFaces;
    ParseObj(fileName, gVertices, gTextures, gNormals, gFaces);
    
    //Parser utilizes the structs defined. While sending OpenGL we will need the base data
    //This is actually not neeeded as the structs we use are homogeneous, so there won't be stride but
    //for clarity I create buffers of base types.
    std::vector<glm::vec3> posData(gVertices.size());
    std::vector<glm::vec2> texData(gTextures.size());
    std::vector<glm::vec3> normalData(gNormals.size());
    //Compute sizes (size: how many bytes?)
    unsigned long posSize = posData.size() * sizeof(glm::vec3);
    unsigned long texSize = texData.size() * sizeof(glm::vec2);
    unsigned long normalSize = normalData.size() * sizeof(glm::vec3);
    //Position Data
    for(int i = 0; i < posData.size(); ++i)
    {
        posData[i].x = gVertices[i].x;
        posData[i].y = gVertices[i].y;
        posData[i].z = gVertices[i].z;
    }
    //Texture Data
    for(int i = 0; i < texData.size(); ++i)
    {
        texData[i].x = gTextures[i].u;
        texData[i].y = gTextures[i].v;
    }
    //Normal Data
    for(int i = 0; i < normalData.size(); ++i)
    {
        normalData[i].x = gNormals[i].x;
        normalData[i].y = gNormals[i].y;
        normalData[i].z = gNormals[i].z;
    }
    

    //Now, data is loaded. Set up the OpenGL buffers
    //Generate the OpenGL objects
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    //Setup the batched data
    glBindVertexArray(VAO);
    //Setup VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //Allocate the buffer
    glBufferData(GL_ARRAY_BUFFER, posSize + texSize + normalSize, 0, GL_STATIC_DRAW);
    //Write the position data
    glBufferSubData(GL_ARRAY_BUFFER, 0, posSize, posData.data());
    //Write the texture data
    glBufferSubData(GL_ARRAY_BUFFER, posSize, texSize, texData.data());
    //Write the normal data
    glBufferSubData(GL_ARRAY_BUFFER, posSize + texSize, normalSize, normalData.data());
    
    //Configure vertex attributes
    /*
       Note that some attributes might not exist (As far as I see given obj files do not have textures given)
       So, except position info I will check whether the data exist or not before configuring the attribute.
       Even though, an attribute do not exist predetermined location will be saved for it.
    */
    
    //Note that as the data is batched, each attribute is tightly packed. So, stride parameter of 0 is sufficient
    //OpenGL assumes the data is tightly packed if stride is 0 and figures out the stride from the attribute size
    //Position Attribute (must exist)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    //Texture Attribute (might not exist)
    if(texSize != 0)
    {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)posSize);
    }
    //Normal Attribute (might not exist)
    if(normalSize != 0)
    {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)(posSize+texSize));
    }
  
    //From the face data retrieve the indexData
    std::vector<glm::ivec3> indexData(gFaces.size());
    unsigned long indexSize = indexData.size() * sizeof(glm::ivec3);
    numTriangles = indexData.size();
    for(int i = 0; i < indexData.size(); ++i)
    {
        indexData[i].x = gFaces[i].vIndex[0];
        indexData[i].y = gFaces[i].vIndex[1];
        indexData[i].z = gFaces[i].vIndex[2];
    }
    //Send the index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indexData.data(), GL_STATIC_DRAW);
    
}


GLuint Mesh::getVAO() const
{
    return VAO;
}

int Mesh::getNumTriangles() const
{
    return numTriangles;
}

//OpenGL Core Libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//stb_image
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


//My headers
#include "Utilities.h"
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"



//Utility Headers
#include <iostream>
#include <vector>
#include <memory>


//Camera
Camera camera(glm::vec3(0.0f, 39.0f, 0.0f));
float near = 1.0f;
float far = 1000.0f;


//Time parameters
double deltaTime = 0.0;
double lastFrame = 0.0;

//Window
GLFWwindow* window;


//Skybox
Mesh skyboxMesh;
Shader skyboxShader;
GLuint skyboxTextureID;


//Quad
Mesh quadMesh;
Shader quadShader;


//Sky Slab
float slabLowerLimit = 40.0f;
float slabUpperLimit = 60.0f;


//Texture Loader
GLuint loadCubemap(const std::vector<std::string>& faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (int i = 0; i < faces.size(); ++i)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


void updateDeltaTime()
{
	double currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

//Callback function in case of resizing the window
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//Function that will process the inputs, such as keyboard inputs
void processInput(GLFWwindow* window)
{
	//If pressed glfwGetKey return GLFW_PRESS, if not it returns GLFW_RELEASE
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	int speedUp = 1; //Default

	//If shift is pressed move the camera faster
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		speedUp = 2;	
	
	//Camera movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(FORWARD, deltaTime, speedUp);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(BACKWARD, deltaTime, speedUp);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(LEFT, deltaTime, speedUp);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(RIGHT, deltaTime, speedUp);



	//Camera y-axis movement
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.moveCameraUp(deltaTime, speedUp);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camera.moveCameraDown(deltaTime, speedUp);

}

//Callback function for mouse position inputs
void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		camera.processMouseMovement(xPos, yPos, GL_TRUE);
	}

	camera.setLastX(xPos);
	camera.setLastY(yPos);	
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.processMouseScroll(yOffset);
}


int setupDependencies()
{
	glfwInit();
	//Specify the version and the OpenGL profile. We are using version 3.3
	//Note that these functions set features for the next call of glfwCreateWindow
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

	//Create the window object
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Window", NULL, NULL);
	if (window == nullptr)
	{
		std::cout << "Failed to create the window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

    // Initialize GLEW to setup the OpenGL Function pointers
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }
    
	//Specify the actual window rectangle for renderings.
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	//Register our size callback funtion to GLFW.
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//GLFW will capture the mouse and will hide the cursor
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Configure Global OpenGL State
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	return 0;
}



//For testing purposes.
GLuint screenSizeQuad()
{
	GLfloat vertices[] = 
	{
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,  // top right
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom left
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f   // top left
	};

	GLuint indices[] = 
	{  
		2, 1, 0,  // first Triangle
		2, 0, 3   // second Triangle
	};

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	//Bind VAO
	glBindVertexArray(VAO);
	//Bind VBO, send data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//Bind EBO, send indices 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	//Configure Vertex Attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3*sizeof(float)));

	//Data passing and configuration is done 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}

void renderTestRectangle(GLuint VAO, Shader shader)
{
	shader.use();
    glm::vec3 camPos = camera.getPosition();
    glm::vec3 lower_left = camPos + near * camera.getFront();
    lower_left += (SCR_WIDTH / 2.0f) * (-camera.getRight()) + (SCR_HEIGHT/2.0f) * (-camera.getUp());
    //Uniforms
    shader.setVec2("resolution", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
    shader.setVec3("camera_pos", camPos);
    shader.setVec3("front", camera.getFront());
    shader.setVec3("right", camera.getRight());
    shader.setVec3("up", camera.getUp());
    shader.setVec3("lower_left", lower_left);
	shader.setFloat("time", glfwGetTime());
	shader.setFloat("lower_limit", slabLowerLimit);
	shader.setFloat("upper_limit", slabUpperLimit);
	glBindVertexArray(VAO);
	//total 6 indices since we have triangles
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}


void loadScene()
{
    //The Skybox
    skyboxMesh = Mesh("hw3_support_files/obj/cube.obj");
    skyboxShader = Shader("Shaders/skyboxShader/skybox_shader_vertex.glsl",
                          "Shaders/skyboxShader/skybox_shader_fragment.glsl");
    
    std::vector<std::string> faces
    {
        std::string("hw3_support_files/skybox_texture_sea/right.jpg"),
        std::string("hw3_support_files/skybox_texture_sea/left.jpg"),
        std::string("hw3_support_files/skybox_texture_sea/top.jpg"),
        std::string("hw3_support_files/skybox_texture_sea/bottom.jpg"),
        std::string("hw3_support_files/skybox_texture_sea/front.jpg"),
        std::string("hw3_support_files/skybox_texture_sea/back.jpg"),
    };
    
    skyboxTextureID = loadCubemap(faces);
    
    
    //Quad
    //quadMesh = Mesh("hw3_support_files/obj/quad.obj");
    //quadShader = Shader("Shaders/solidColor/solidColor.vert",
                          //"Shaders/solidColor/solidColor.frag");
    
}

void renderSkybox(const Mesh& mesh, Shader& shader)
{
    glDepthFunc(GL_LEQUAL);
    shader.use();
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.getFov()), (float)SCR_WIDTH / SCR_HEIGHT, near, far);
    glm::mat4 PV = projection * glm::mat4(glm::mat3(view));
    //Set uniforms
    shader.setMat4("PV", PV);
    //Set skybox texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
    glBindVertexArray(mesh.getVAO());
    glDrawElements(GL_TRIANGLES, 3 * mesh.getNumTriangles(), GL_UNSIGNED_INT, 0);
    glDepthFunc(GL_LESS);
}

void renderQuad(const Mesh& mesh, Shader& shader)
{
    shader.use();
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.getFov()), (float)SCR_WIDTH / SCR_HEIGHT, near, far);
    //Set uniforms
    glBindVertexArray(mesh.getVAO());
    glDrawElements(GL_TRIANGLES, 3 * mesh.getNumTriangles(), GL_UNSIGNED_INT, 0);
}

void renderScene()
{
    renderSkybox(skyboxMesh, skyboxShader);
}


int main()
{
	setupDependencies();
    GLuint rect = screenSizeQuad();
	Shader shader("Shaders/cloudShader/cloud_shader_vertex.glsl",
                  "Shaders/cloudShader/cloud_shader_fragment.glsl");
    
    loadScene();

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		//Update deltaTime
		updateDeltaTime();
		// input
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderScene();
        renderTestRectangle(rect, shader);
        
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();

	return 0;
}



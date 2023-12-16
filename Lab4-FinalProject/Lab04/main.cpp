// Windows includes (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>
// Assimp includes
#include <assimp/cimport.h> // scene importer 
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

// Include GLM functions
#include <C:/Users/M2-Winterfell/Downloads/CSU44052-Computer-Graphics/Lab4-FinalProject/libs/glm/glm.hpp>
#include <C:/Users/M2-Winterfell/Downloads/CSU44052-Computer-Graphics/Lab4-FinalProject/libs/glm/gtc/matrix_transform.hpp>
#include <C:/Users/M2-Winterfell/Downloads/CSU44052-Computer-Graphics/Lab4-FinalProject/libs/glm/gtc/type_ptr.hpp>

// Project includes
#include "maths_funcs.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// Meshes to Load
#define CASTLE_NAME		"models/castle.dae"
#define GROUND_NAME		"models/plane.dae"
#define TOWER_NAME		"models/watchtower.dae"
#define GRASS_NAME		"models/vegetation.dae"
#define BELL_NAME		"models/bell.dae"
#define LAMP_NAME		"models/street_light.dae"
#define MOON_NAME		"models/moon.dae"
#define ZOMBIE_NAME		"models/carcass_eater.dae"
#define ZOMBIE2_NAME	"models/swampGhoul.dae"
// Textures to Load
#define CASTLE_TEXTURE	"textures/yellow_brick.jpg"
#define	GROUND_TEXTURE	"textures/grass.jpg"
#define	TOWER_TEXTURE	"textures/creamy_brick.jpg"
#define	GRASS_TEXTURE	"textures/grass.jpg"
#define	BELL_TEXTURE	"textures/milk_brick.jpg"
#define	LAMP_TEXTURE	"textures/metal.jpg"
#define	MOON_TEXTURE	"textures/moon_texture.jpg"
#define	ZOMBIE_TEXTURE	"textures/eater.jpg"
#define	ZOMBIE2_TEXTURE	"textures/swampGhoul.jpg"
// Sounds to Load
//#define	AMBIENT			"sounds/475635__o-ciz__forest-windy.wav"

// Camera
glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 15.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float lastX, lastY;
float yaw = -90.0f;
float pitch = 0.0f;

// Key states for W, A, S, D, Q, E
bool keyW = false;
bool keyA = false;
bool keyS = false;
bool keyD = false;
bool keyQ = false;
bool keyE = false;
// Mouse button state
bool leftButtonPressed = false;

#pragma region SimpleTypes
typedef struct
{
	size_t mPointCount = 0;
	std::vector<vec3> mVertices;
	std::vector<vec3> mNormals;
	std::vector<vec2> mTextureCoords;
} ModelData;
#pragma endregion SimpleTypes

using namespace std;

// Shaders
GLuint shaderProgramID;
// Screen
int width = 1120;
int height = 630;
// Models
ModelData castle, ground, tower, bell;
ModelData veg, lamp, moon, zombie, zombie2;
// Textures
unsigned int CASTLE_TEXTURE_tex, GROUND_tex, TOWER_TEXTURE_tex, GRASS_tex, BELL_tex, LAMP_tex, MOON_tex, ZOMBIE_tex, ZOMBIE2_tex;
// Buffers
unsigned int VP_VBOs[14]; // vertex positions
unsigned int VN_VBOs[14]; // vertex normals
unsigned int VT_VBOs[14]; // vertex textures
unsigned int VAOs[1];

GLuint loc1[8 * 3];
GLuint loc2[6 * 3];

GLfloat rotate_y = 0.0f;
GLfloat rotate_x = 0.0f;
GLfloat rotate_z = 0.0f;

// translation
float translation_x = -2.0f;
float translation_y = 0.05f;
float translation_z = 2.0f;
// Camera
float camera_x = 0.0f;
float camera_y = 5.0f;
float camera_z = 15.0f;
float target_x = 0.0f;
float target_y = 0.0f;
float target_z = 0.0f;
vec3 camera_pos = vec3(0.0f, 0.0f, 0.0f);		// initial position of eye
vec3 camera_target = vec3(0.0f, 0.0f, 0.0f);	// initial position of target
vec3 up = vec3(0.0f, 1.0f, 0.0f);				// up vector

void mouseMove(int x, int y) {
	//if (!leftButtonPressed) return;

	if (firstMouse) {
		lastX = x;
		lastY = y;
		firstMouse = false;
	}

	float xoffset = x - lastX;
	float yoffset = lastY - y; // reversed since y-coordinates go from bottom to top
	lastX = x;
	lastY = y;

	float sensitivity = 0.2f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

// Mouse button callback function
void mouseButton(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			leftButtonPressed = true;
			firstMouse = true; // Re-initialize to true to update lastX and lastY with the current position
		}
		else if (state == GLUT_UP) {
			leftButtonPressed = false;
		}
	}
}

void keypress(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
	case 'W':
		keyW = true;
		break;
	case 'a':
	case 'A':
		keyA = true;
		break;
	case 's':
	case 'S':
		keyS = true;
		break;
	case 'd':
	case 'D':
		keyD = true;
		break;
	case 'q':
	case 'Q':
		keyQ = true;
		break;
	case 'e':
	case 'E':
		keyE = true;
		break;
	}
}

void keyUp(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
	case 'W':
		keyW = false;
		break;
	case 'a':
	case 'A':
		keyA = false;
		break;
	case 's':
	case 'S':
		keyS = false;
		break;
	case 'd':
	case 'D':
		keyD = false;
		break;
	case 'q':
	case 'Q':
		keyQ = false;
		break;
	case 'e':
	case 'E':
		keyE = false;
		break;
	}
}

// Define a structure to hold spotlight properties
struct Spotlight {
	glm::vec3 position;
	glm::vec3 direction;
	float cutOff;
	float outerCutOff;
	glm::vec3 color;
};

Spotlight spotlights[4];

// 3d Perspective
mat4 persp_proj = perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
// Loads Textures using ASSIMP
#pragma region MESH LOADING
/*----------------------------------------------------------------------------
MESH LOADING FUNCTION
----------------------------------------------------------------------------*/

ModelData load_mesh(const char* file_name, float texture_scale = 1.0f) {
	ModelData modelData;

	/* Use assimp to read the model file, forcing it to be read as    */
	/* triangles. The second flag (aiProcess_PreTransformVertices) is */
	/* relevant if there are multiple meshes in the model file that   */
	/* are offset from the origin. This is pre-transform them so      */
	/* they're in the right position.                                 */

	const aiScene* scene = aiImportFile(
		file_name,
		aiProcess_Triangulate | aiProcess_PreTransformVertices
	);

	if (!scene) {
		fprintf(stderr, "ERROR: reading mesh %s\n", file_name);
		return modelData;
	}

	printf("  %i materials\n", scene->mNumMaterials);
	printf("  %i meshes\n", scene->mNumMeshes);
	printf("  %i textures\n", scene->mNumTextures);

	for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
		const aiMesh* mesh = scene->mMeshes[m_i];
		printf("    %i vertices in mesh\n", mesh->mNumVertices);
		modelData.mPointCount += mesh->mNumVertices;
		for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
			if (mesh->HasPositions()) {
				const aiVector3D* vp = &(mesh->mVertices[v_i]);
				modelData.mVertices.push_back(vec3(vp->x, vp->y, vp->z));
			}
			if (mesh->HasNormals()) {
				const aiVector3D* vn = &(mesh->mNormals[v_i]);
				modelData.mNormals.push_back(vec3(vn->x, vn->y, vn->z));
			}
			if (mesh->HasTextureCoords(0)) {
				const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
				modelData.mTextureCoords.push_back(vec2(vt->x * texture_scale, vt->y * texture_scale));
			}
			if (mesh->HasTangentsAndBitangents()) {
				/* You can extract tangents and bitangents here              */
				/* Note that you might need to make Assimp generate this     */
				/* data for you. Take a look at the flags that aiImportFile  */
				/* can take.                                                 */
			}
		}
	}
	aiReleaseImport(scene);
	return modelData;
}

#pragma endregion MESH LOADING
// Loads Textures using STB Image
#pragma region TEXTURE LOADING
unsigned int load_tex(const char* file_name) {
	// load and create a texture 
	unsigned int texture = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load(file_name, &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	return texture;
}
#pragma endregion TEXTURE LOADING
// Shader Functions
#pragma region SHADER_FUNCTIONS
char* readShaderSource(const char* shaderFile) {
	FILE* fp;
	fopen_s(&fp, shaderFile, "rb");
	if (fp == NULL) { return NULL; }
	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	buf[size] = '\0';
	fclose(fp);
	return buf;
}


static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		std::cerr << "Error creating shader..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	const char* pShaderSource = readShaderSource(pShaderText);

	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	// compile the shader and check for errors
	glCompileShader(ShaderObj);
	GLint success;
	// check for shader related errors using glGetShaderiv
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024] = { '\0' };
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		std::cerr << "Error compiling "
			<< (ShaderType == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< " shader program: " << InfoLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	// Attach the compiled shader object to the program object
	glAttachShader(ShaderProgram, ShaderObj);
}

GLuint CompileShaders()
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
	shaderProgramID = glCreateProgram();

	if (shaderProgramID == 0) {
		std::cerr << "Error creating shader program..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// Create two shader objects, one for the vertex, and one for the fragment shader
	AddShader(shaderProgramID, "simpleVertexShader.txt", GL_VERTEX_SHADER);
	AddShader(shaderProgramID, "simpleFragmentShader.txt", GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { '\0' };
	// After compiling all shader objects and attaching them to the program, we can finally link it
	glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Error linking shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
	glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Invalid shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
	glUseProgram(shaderProgramID);
	return shaderProgramID;
}
#pragma endregion SHADER_FUNCTIONS
// VBO Functions 
#pragma region VBO_FUNCTIONS
void generateObjectBufferMesh() {
	// load meshes
	castle = load_mesh(CASTLE_NAME, 5.0f);
	ground = load_mesh(GROUND_NAME, 10.0f);
	tower = load_mesh(TOWER_NAME, 3.0f);
	veg = load_mesh(GRASS_NAME);
	bell = load_mesh(BELL_NAME, 5.0f);
	lamp = load_mesh(LAMP_NAME);
	moon = load_mesh(MOON_NAME);
	zombie = load_mesh(ZOMBIE_NAME);
	zombie2 = load_mesh(ZOMBIE2_NAME);
	// load textures
	CASTLE_TEXTURE_tex = load_tex(CASTLE_TEXTURE);
	GROUND_tex = load_tex(GROUND_TEXTURE);
	TOWER_TEXTURE_tex = load_tex(TOWER_TEXTURE);
	GRASS_tex = load_tex(GRASS_TEXTURE);
	BELL_tex = load_tex(BELL_TEXTURE);
	LAMP_tex = load_tex(LAMP_TEXTURE);
	MOON_tex = load_tex(MOON_TEXTURE);
	ZOMBIE_tex = load_tex(ZOMBIE_TEXTURE);
	ZOMBIE2_tex = load_tex(ZOMBIE2_TEXTURE);
	// Generate Buffers
	glGenBuffers(14, VP_VBOs); // position buffer
	glGenBuffers(14, VN_VBOs); // normal buffer
	glGenBuffers(14, VT_VBOs); // texture buffer


	// Scene - Moon
	loc1[0] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc1[1] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc1[2] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, moon.mPointCount * sizeof(vec3), &moon.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, moon.mPointCount * sizeof(vec3), &moon.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, moon.mPointCount * sizeof(vec2), &moon.mTextureCoords[0], GL_STATIC_DRAW);
	// Scene - Zombie
	loc1[3] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc1[4] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc1[5] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, zombie.mPointCount * sizeof(vec3), &zombie.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, zombie.mPointCount * sizeof(vec3), &zombie.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, zombie.mPointCount * sizeof(vec2), &zombie.mTextureCoords[0], GL_STATIC_DRAW);
	// Dog - Eyelids
	loc1[6] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc1[7] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc1[8] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[2]);
	glBufferData(GL_ARRAY_BUFFER, zombie2.mPointCount * sizeof(vec3), &zombie2.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[2]);
	glBufferData(GL_ARRAY_BUFFER, zombie2.mPointCount * sizeof(vec3), &zombie2.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[2]);
	glBufferData(GL_ARRAY_BUFFER, zombie2.mPointCount * sizeof(vec2), &zombie2.mTextureCoords[0], GL_STATIC_DRAW);
	// Scene - CASTLE
	loc2[0] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2[1] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc2[2] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[8]);
	glBufferData(GL_ARRAY_BUFFER, castle.mPointCount * sizeof(vec3), &castle.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[8]);
	glBufferData(GL_ARRAY_BUFFER, castle.mPointCount * sizeof(vec3), &castle.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[8]);
	glBufferData(GL_ARRAY_BUFFER, castle.mPointCount * sizeof(vec2), &castle.mTextureCoords[0], GL_STATIC_DRAW);
	// Scene - ground
	loc2[3] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2[4] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc2[5] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[9]);
	glBufferData(GL_ARRAY_BUFFER, ground.mPointCount * sizeof(vec3), &ground.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[9]);
	glBufferData(GL_ARRAY_BUFFER, ground.mPointCount * sizeof(vec3), &ground.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[9]);
	glBufferData(GL_ARRAY_BUFFER, ground.mPointCount * sizeof(vec2), &ground.mTextureCoords[0], GL_STATIC_DRAW);
	// Scene - bell
	loc2[6] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2[7] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc2[8] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[10]);
	glBufferData(GL_ARRAY_BUFFER, bell.mPointCount * sizeof(vec3), &bell.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[10]);
	glBufferData(GL_ARRAY_BUFFER, bell.mPointCount * sizeof(vec3), &bell.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[10]);
	glBufferData(GL_ARRAY_BUFFER, bell.mPointCount * sizeof(vec2), &bell.mTextureCoords[0], GL_STATIC_DRAW);
	// Scene - lamp
	loc2[9] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2[10] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc2[11] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[11]);
	glBufferData(GL_ARRAY_BUFFER, lamp.mPointCount * sizeof(vec3), &lamp.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[11]);
	glBufferData(GL_ARRAY_BUFFER, lamp.mPointCount * sizeof(vec3), &lamp.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[11]);
	glBufferData(GL_ARRAY_BUFFER, lamp.mPointCount * sizeof(vec2), &lamp.mTextureCoords[0], GL_STATIC_DRAW);
	// Scene - Vegetation
	loc2[12] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2[13] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc2[14] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[12]);
	glBufferData(GL_ARRAY_BUFFER, veg.mPointCount * sizeof(vec3), &veg.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[12]);
	glBufferData(GL_ARRAY_BUFFER, veg.mPointCount * sizeof(vec3), &veg.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[12]);
	glBufferData(GL_ARRAY_BUFFER, veg.mPointCount * sizeof(vec2), &veg.mTextureCoords[0], GL_STATIC_DRAW);
	// Scene - Kennel
	loc2[15] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2[16] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc2[17] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[13]);
	glBufferData(GL_ARRAY_BUFFER, tower.mPointCount * sizeof(vec3), &tower.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[13]);
	glBufferData(GL_ARRAY_BUFFER, tower.mPointCount * sizeof(vec3), &tower.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[13]);
	glBufferData(GL_ARRAY_BUFFER, tower.mPointCount * sizeof(vec2), &tower.mTextureCoords[0], GL_STATIC_DRAW);
}
#pragma endregion VBO_FUNCTIONS

void display() {
	// Tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthMask(GL_TRUE); //update the depth buffer
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.4f, 0.06f, 0.0f, 1.0f); // Darker scarlet color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	// Declare your uniform variables that will be used in your shader
	int matrix_location = glGetUniformLocation(shaderProgramID, "model");
	int view_mat_location = glGetUniformLocation(shaderProgramID, "view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID, "proj");
	// Additional Uniforms for lighting
	int lightPos_location = glGetUniformLocation(shaderProgramID, "lightPos");
	int viewPos_location = glGetUniformLocation(shaderProgramID, "viewPos");
	int lightColor_location = glGetUniformLocation(shaderProgramID, "lightColor");

	// Set light properties
	glm::vec3 lightPos(-25.0f, 30.0f, -25.0f); // Change as needed
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f); // white light
	spotlights[0] = { glm::vec3(2.2f, 4.0f, 2.2f), glm::vec3(0.0f, -1.0f, 0.0f), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)), glm::vec3(1.0f, 1.0f, 1.0f) };
	spotlights[1] = { glm::vec3(2.2f, 4.0f, -2.2f), glm::vec3(0.0f, -1.0f, 0.0f), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)), glm::vec3(1.0f, 1.0f, 1.0f) };
	spotlights[2] = { glm::vec3(-2.2f, 4.0f, 2.2f), glm::vec3(0.0f, -1.0f, 0.0f), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)), glm::vec3(1.0f, 1.0f, 1.0f) };
	spotlights[3] = { glm::vec3(-2.2f, 4.0f, -2.2f), glm::vec3(0.0f, -1.0f, 0.0f), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)), glm::vec3(1.0f, 1.0f, 1.0f) };

	// In your display function, pass the spotlight properties to the shader
	for (int i = 0; i < 4; i++) {
		std::string uniformName = "spotlights[" + std::to_string(i) + "]";
		glUniform3fv(glGetUniformLocation(shaderProgramID, (uniformName + ".position").c_str()), 1, glm::value_ptr(spotlights[i].position));
		glUniform3fv(glGetUniformLocation(shaderProgramID, (uniformName + ".direction").c_str()), 1, glm::value_ptr(spotlights[i].direction));
		glUniform1f(glGetUniformLocation(shaderProgramID, (uniformName + ".cutOff").c_str()), spotlights[i].cutOff);
		glUniform1f(glGetUniformLocation(shaderProgramID, (uniformName + ".outerCutOff").c_str()), spotlights[i].outerCutOff);
		glUniform3fv(glGetUniformLocation(shaderProgramID, (uniformName + ".color").c_str()), 1, glm::value_ptr(spotlights[i].color));
	}

	// Camera / View transformation
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	// Update view and projection matrices in the shader
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, glm::value_ptr(projection));
	// Pass light to the shader
	glUniform3fv(lightPos_location, 1, glm::value_ptr(lightPos));
	glUniform3fv(viewPos_location, 1, glm::value_ptr(cameraPos));
	glUniform3fv(lightColor_location, 1, glm::value_ptr(lightColor));

	// Moon
	mat4 moon_model = identity_mat4();
	moon_model = translate(moon_model, vec3(-5.2f, 6.2f, -5.2f));
	moon_model = scale(moon_model, vec3(5.0f, 5.0f, 5.0f));
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, moon_model.m);
	glEnableVertexAttribArray(loc1[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[0]);
	glVertexAttribPointer(loc1[0], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc1[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[0]);
	glVertexAttribPointer(loc1[1], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(VAOs[0]);
	glEnableVertexAttribArray(loc1[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[0]);
	glVertexAttribPointer(loc1[2], 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindTexture(GL_TEXTURE_2D, MOON_tex);
	glDrawArrays(GL_TRIANGLES, 0, moon.mPointCount);
	// Zombie
	mat4 zombie_model = identity_mat4();
	zombie_model = rotate_y_deg(zombie_model, 180.0f); // Apply rotation
	zombie_model = translate(zombie_model, vec3(-10.0f, 0.5f, 0.0f));
	zombie_model = scale(zombie_model, vec3(1.0f, 1.0f, 1.0f));
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, zombie_model.m);
	glEnableVertexAttribArray(loc1[3]);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[1]);
	glVertexAttribPointer(loc1[3], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc1[4]);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[1]);
	glVertexAttribPointer(loc1[4], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(VAOs[0]);
	glEnableVertexAttribArray(loc1[5]);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[1]);
	glVertexAttribPointer(loc1[5], 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindTexture(GL_TEXTURE_2D, ZOMBIE_tex);
	glDrawArrays(GL_TRIANGLES, 0, zombie.mPointCount);
	// Zombie 2
	mat4 zombie_model2 = identity_mat4();
	zombie_model2 = rotate_y_deg(zombie_model2, 0.0f); // Apply rotation
	zombie_model2 = translate(zombie_model2, vec3(5.0f, 1.0f, 0.0f));
	zombie_model2 = scale(zombie_model2, vec3(2.0f, 2.0f, 2.0f));
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, zombie_model2.m);
	glEnableVertexAttribArray(loc1[6]);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[2]);
	glVertexAttribPointer(loc1[6], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc1[7]);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[2]);
	glVertexAttribPointer(loc1[7], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(VAOs[0]);
	glEnableVertexAttribArray(loc1[8]);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[2]);
	glVertexAttribPointer(loc1[8], 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindTexture(GL_TEXTURE_2D, ZOMBIE2_tex);
	glDrawArrays(GL_TRIANGLES, 0, zombie2.mPointCount);
	// Root - Scene - CASTLE
	mat4 bg = identity_mat4();
	bg = translate(bg, vec3(0.0f, -10.0f, 0.0f));
	bg = scale(bg, vec3(0.001f, 0.001f, 0.001f)); // Add the scaling transformation here
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, bg.m);
	glEnableVertexAttribArray(loc2[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[8]);
	glVertexAttribPointer(loc2[0], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[8]);
	glVertexAttribPointer(loc2[1], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(VAOs[0]);
	glEnableVertexAttribArray(loc2[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[8]);
	glVertexAttribPointer(loc2[2], 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindTexture(GL_TEXTURE_2D, CASTLE_TEXTURE_tex);
	glDrawArrays(GL_TRIANGLES, 0, castle.mPointCount);
	// Child - Scene - ground
	mat4 bgChild = identity_mat4();
	bgChild = scale(bgChild, vec3(60.0f, 60.0f, 60.0f)); // Add the scaling transformation here
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, bgChild.m);
	glEnableVertexAttribArray(loc2[3]);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[9]);
	glVertexAttribPointer(loc2[3], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2[4]);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[9]);
	glVertexAttribPointer(loc2[4], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(VAOs[0]);
	glEnableVertexAttribArray(loc2[5]);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[9]);
	glVertexAttribPointer(loc2[5], 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindTexture(GL_TEXTURE_2D, GROUND_tex);
	glDrawArrays(GL_TRIANGLES, 0, ground.mPointCount);
	// Child - Scene - bell
	mat4 belltower = identity_mat4();
	belltower = translate(belltower, vec3(0.0f, 0.5f, 0.0f));
	belltower = scale(belltower, vec3(3.0f, 3.0f, 3.0f)); // Add the scaling transformation here
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, belltower.m);
	glEnableVertexAttribArray(loc2[6]);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[10]);
	glVertexAttribPointer(loc2[6], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2[7]);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[10]);
	glVertexAttribPointer(loc2[7], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(VAOs[0]);
	glEnableVertexAttribArray(loc2[8]);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[10]);
	glVertexAttribPointer(loc2[8], 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindTexture(GL_TEXTURE_2D, BELL_tex);
	glDrawArrays(GL_TRIANGLES, 0, bell.mPointCount);
	// Child - Scene - lamp
	// Array of positions for lamp instances
	vec3 lampPositions[] = {
		vec3(12.0f, 0.0f, -12.0f),  // Original position
		vec3(-12.0f, 0.0f, -12.0f),  // top left position
		vec3(12.0f, 0.0f,  12.0f),  // bottom right position
		vec3(-12.0f, 0.0f,  12.0f)   // bottom left position
	};

	for (int i = 0; i < 4; ++i) {
		mat4 light = identity_mat4();
		light = translate(light, lampPositions[i]);
		light = scale(light, vec3(0.2f, 0.2f, 0.2f)); // Scale can be adjusted if needed
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, light.m);
		glEnableVertexAttribArray(loc2[9]);
		glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[11]);
		glVertexAttribPointer(loc2[9], 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(loc2[10]);
		glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[11]);
		glVertexAttribPointer(loc2[10], 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindVertexArray(VAOs[0]);
		glEnableVertexAttribArray(loc2[11]);
		glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[11]);
		glVertexAttribPointer(loc2[11], 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindTexture(GL_TEXTURE_2D, LAMP_tex);
		glDrawArrays(GL_TRIANGLES, 0, lamp.mPointCount);
	}

	// Root - Scene - Vegetation
	mat4 grass = identity_mat4();
	grass = scale(grass, vec3(0.2f, 0.2f, 0.2f)); // Scale can be adjusted if needed
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, grass.m);
	glEnableVertexAttribArray(loc2[12]);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[12]);
	glVertexAttribPointer(loc2[12], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2[13]);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[12]);
	glVertexAttribPointer(loc2[13], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(VAOs[0]);
	glEnableVertexAttribArray(loc2[14]);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[12]);
	glVertexAttribPointer(loc2[14], 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindTexture(GL_TEXTURE_2D, GRASS_tex);
	glDrawArrays(GL_TRIANGLES, 0, veg.mPointCount);
	// Root - Scene - Kennel

	// Array of positions for kennel instances
	vec3 kennelPositions[] = {
		vec3(11.0f, 0.0f, -11.0f),  // Original position
		vec3(-11.0f, 0.0f, -11.0f),  // top left position
		vec3(9.0f, 0.0f,  11.5f),  // bottom right position
		vec3(-12.0f, 0.0f,  11.0f)   // bottom left position
	};

	// Array of rotation angles (in degrees) for each kennel instance
	float kennelRotations[] = {
		rotate_y,       // Rotation for the first kennel
		rotate_y,  // Rotation for the second kennel, and so on...
		rotate_y + 180,
		rotate_y + 180
	};

	for (int i = 0; i < 4; ++i) {
		mat4 kennelMat = identity_mat4();
		kennelMat = rotate_y_deg(kennelMat, kennelRotations[i]); // Apply rotation
		kennelMat = translate(kennelMat, kennelPositions[i]);
		kennelMat = scale(kennelMat, vec3(2.0f, 2.0f, 2.0f)); // Scale can be adjusted if needed

		glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, kennelMat.m);

		glEnableVertexAttribArray(loc2[15]);
		glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[13]);
		glVertexAttribPointer(loc2[15], 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(loc2[16]);
		glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[13]);
		glVertexAttribPointer(loc2[16], 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindVertexArray(VAOs[0]);

		glEnableVertexAttribArray(loc2[17]);
		glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[13]);
		glVertexAttribPointer(loc2[17], 2, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindTexture(GL_TEXTURE_2D, TOWER_TEXTURE_tex);
		glDrawArrays(GL_TRIANGLES, 0, tower.mPointCount);
	}
	glutSwapBuffers();
}


void updateScene() {
	// Delta Calculation
	static DWORD last_time = 0;
	DWORD curr_time = timeGetTime();
	if (last_time == 0)
		last_time = curr_time;
	float delta = (curr_time - last_time) * 0.001f;
	last_time = curr_time;

	float cameraSpeed = 0.1f; // adjust accordingly

	glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));

	// Forward and backward movement
	if (keyW) cameraPos += cameraSpeed * cameraFront;
	if (keyS) cameraPos -= cameraSpeed * cameraFront;

	// Left and right movement
	if (keyA) cameraPos -= cameraSpeed * cameraRight;
	if (keyD) cameraPos += cameraSpeed * cameraRight;

	// Up and down movement
	if (keyQ) cameraPos += cameraSpeed * cameraUp;
	if (keyE) cameraPos -= cameraSpeed * cameraUp;

	// Diagonal movement
	if (keyW && keyA) cameraPos += glm::normalize(glm::cross(cameraUp, cameraFront)) * cameraSpeed;
	if (keyW && keyD) cameraPos -= glm::normalize(glm::cross(cameraUp, cameraFront)) * cameraSpeed;
	if (keyS && keyA) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keyS && keyD) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	// Proximity Calculation
	vec3 dist = vec3(translation_x, translation_y, translation_z) - vec3(camera_x, camera_y, camera_z);
	float x_distance = dist.v[0];
	float z_distance = dist.v[2];
	// Draw the next frame
	glutPostRedisplay();
}

void init()
{
	// Set up the shaders
	GLuint shaderProgramID = CompileShaders();
	// Load mesh into a vertex buffer array
	glutKeyboardFunc(keypress);
	glutKeyboardUpFunc(keyUp);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove); // Register mouse move callback
	generateObjectBufferMesh();
}



int main(int argc, char** argv) {
	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Display");
	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutWarpPointer(width / 2, height / 2);
	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	//PlaySound(TEXT(AMBIENT), NULL, SND_ASYNC);
	glutMainLoop();
	return 0;
}
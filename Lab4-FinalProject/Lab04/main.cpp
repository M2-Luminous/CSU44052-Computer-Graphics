#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector>

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
#define SKY_NAME		"models/plane.dae"
#define BELL_NAME		"models/bell.dae"
#define LAMP_NAME		"models/street_light.dae"
#define SEA_NAME		"models/ground.dae"
#define ZOMBIE_NAME		"models/carcass_eater.dae"
#define ZOMBIE2_NAME	"models/swampGhoul.dae"
#define ZOMBIE3_NAME	"models/skeleton.dae"
#define HUMAN_NAME		"models/human.dae"
#define MONSTER_NAME	"models/monster_selfmade.dae"
#define BAT_NAME		"models/bat_selfmade.dae"
// Textures to Load
#define CASTLE_TEXTURE		"textures/yellow_brick.jpg"
#define	GROUND_TEXTURE		"textures/grass.jpg"
#define	TOWER_TEXTURE		"textures/creamy_brick.jpg"
#define	SKY_TEXTURE			"textures/star-sky.jpg"
#define	BELL_TEXTURE		"textures/milk_brick.jpg"
#define	LAMP_TEXTURE		"textures/metal.jpg"
#define	SEA_TEXTURE			"textures/ocean.jpg"
#define	ZOMBIE_TEXTURE		"textures/eater.jpg"
#define	ZOMBIE2_TEXTURE		"textures/swampGhoul.jpg"
#define	ZOMBIE3_TEXTURE		"textures/bone.jpg"
#define	HUMAN_TEXTURE		"textures/skin.jpg"
#define	MONSTER_TEXTURE		"textures/monster_skin.jpg"
#define	BAT_TEXTURE			"textures/bat_skin.jpg"
// Sounds to Load
#define	AMBIENT			"sounds/apocalypse.wav"

// Global variable
int startTime = 0;
float rotationSpeed = 4.0f; // Adjust the speed of rotation as needed

// human position variables
float humanPosX = 0.0f;
float humanPosZ = 0.0f;
float zombiePosX = -10.0f; // Adjust these initial values as needed
float zombiePosZ = 0.0f;
float zombie2PosX = 5.0f; // Adjust these initial values as needed
float zombie2PosZ = 0.0f;

float humanRotationAngle = 0.0f; // Rotation angle in degrees
float zombieRotationAngle = 0.0f;
float zombie2RotationAngle = 0.0f;
float zombie3RotationAngle = 0.0f;

float translationSpeed = 0.03f; // Adjust as needed
float movementSpeed = 0.015f; // Adjust as needed for smooth chasing
float collisionDistance = 6.0f; // Adjust as per the size of models
// Check for collision with obstacles
glm::vec3 obstacle1Pos(0.0f, 0.0f, 0.0f);
glm::vec3 obstacle2Pos(zombiePosX, 0.0f, zombiePosZ);
glm::vec3 obstacle3Pos(zombie2PosX, 0.0f, zombie2PosZ);
glm::vec3 obstacle4Pos(humanPosX, 0.0f, humanPosZ);

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
// Arrow key states
bool KeyUp = false;
bool KeyDown = false;
bool KeyLeft = false;
bool KeyRight = false;
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
ModelData sky, lamp, sea, zombie, zombie2, zombie3, human, monster, bat;
// Terrain generation
// ModelData terrain = generateTerrain(100, 100, 1.0f); // 100x100 grid with 1.0f size for each square
// Textures
unsigned int CASTLE_tex, GROUND_tex, TOWER_tex, SKY_tex, BELL_tex, LAMP_tex, SEA_tex;
unsigned int ZOMBIE_tex, ZOMBIE2_tex, ZOMBIE3_tex, HUMAN_tex, MONSTER_tex, BAT_tex;
// Buffers
unsigned int VP_VBOs[14]; // vertex positions
unsigned int VN_VBOs[14]; // vertex normals
unsigned int VT_VBOs[14]; // vertex textures
unsigned int VAOs[1];

GLuint loc1[8 * 3];
GLuint loc2[6 * 3];

// Camera
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

void specialKeyPress(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		KeyUp = true;
		break;
	case GLUT_KEY_DOWN:
		KeyDown = true;
		break;
	case GLUT_KEY_LEFT:
		KeyLeft = true;
		break;
	case GLUT_KEY_RIGHT:
		KeyRight = true;
		break;
	}
}

void specialKeyUp(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		KeyUp = false;
		break;
	case GLUT_KEY_DOWN:
		KeyDown = false;
		break;
	case GLUT_KEY_LEFT:
		KeyLeft = false;
		break;
	case GLUT_KEY_RIGHT:
		KeyRight = false;
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
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
	sky = load_mesh(SKY_NAME);
	bell = load_mesh(BELL_NAME, 5.0f);
	lamp = load_mesh(LAMP_NAME);
	sea = load_mesh(SEA_NAME);
	zombie = load_mesh(ZOMBIE_NAME);
	zombie2 = load_mesh(ZOMBIE2_NAME);
	zombie3 = load_mesh(ZOMBIE3_NAME);
	human = load_mesh(HUMAN_NAME);
	monster = load_mesh(MONSTER_NAME);
	bat = load_mesh(BAT_NAME);
	// load textures
	CASTLE_tex = load_tex(CASTLE_TEXTURE);
	GROUND_tex = load_tex(GROUND_TEXTURE);
	TOWER_tex = load_tex(TOWER_TEXTURE);
	SKY_tex = load_tex(SKY_TEXTURE);
	BELL_tex = load_tex(BELL_TEXTURE);
	LAMP_tex = load_tex(LAMP_TEXTURE);
	SEA_tex = load_tex(SEA_TEXTURE);
	ZOMBIE_tex = load_tex(ZOMBIE_TEXTURE);
	ZOMBIE2_tex = load_tex(ZOMBIE2_TEXTURE);
	ZOMBIE3_tex = load_tex(ZOMBIE3_TEXTURE);
	HUMAN_tex = load_tex(HUMAN_TEXTURE);
	MONSTER_tex = load_tex(MONSTER_TEXTURE);
	BAT_tex = load_tex(BAT_TEXTURE);
	// Generate Buffers
	glGenBuffers(14, VP_VBOs); // position buffer
	glGenBuffers(14, VN_VBOs); // normal buffer
	glGenBuffers(14, VT_VBOs); // texture buffer


	// Scene - Sea
	loc1[0] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc1[1] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc1[2] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sea.mPointCount * sizeof(vec3), &sea.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sea.mPointCount * sizeof(vec3), &sea.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sea.mPointCount * sizeof(vec2), &sea.mTextureCoords[0], GL_STATIC_DRAW);
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
	// Scene - Zombie2
	loc1[6] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc1[7] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc1[8] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[2]);
	glBufferData(GL_ARRAY_BUFFER, zombie2.mPointCount * sizeof(vec3), &zombie2.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[2]);
	glBufferData(GL_ARRAY_BUFFER, zombie2.mPointCount * sizeof(vec3), &zombie2.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[2]);
	glBufferData(GL_ARRAY_BUFFER, zombie2.mPointCount * sizeof(vec2), &zombie2.mTextureCoords[0], GL_STATIC_DRAW);
	// Scene - Skeleton
	loc1[9] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc1[10] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc1[11] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[3]);
	glBufferData(GL_ARRAY_BUFFER, human.mPointCount * sizeof(vec3), &human.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[3]);
	glBufferData(GL_ARRAY_BUFFER, human.mPointCount * sizeof(vec3), &human.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[3]);
	glBufferData(GL_ARRAY_BUFFER, human.mPointCount * sizeof(vec2), &human.mTextureCoords[0], GL_STATIC_DRAW);
	// Scene - Zombie3
	loc1[12] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc1[13] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc1[14] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[4]);
	glBufferData(GL_ARRAY_BUFFER, zombie3.mPointCount * sizeof(vec3), &zombie3.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[4]);
	glBufferData(GL_ARRAY_BUFFER, zombie3.mPointCount * sizeof(vec3), &zombie3.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[4]);
	glBufferData(GL_ARRAY_BUFFER, zombie3.mPointCount * sizeof(vec2), &zombie3.mTextureCoords[0], GL_STATIC_DRAW);
	// Scene - Monster
	loc1[15] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc1[16] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc1[17] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[5]);
	glBufferData(GL_ARRAY_BUFFER, monster.mPointCount * sizeof(vec3), &monster.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[5]);
	glBufferData(GL_ARRAY_BUFFER, monster.mPointCount * sizeof(vec3), &monster.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[5]);
	glBufferData(GL_ARRAY_BUFFER, monster.mPointCount * sizeof(vec2), &monster.mTextureCoords[0], GL_STATIC_DRAW);
	// Scene - Bat
	loc1[18] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc1[19] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc1[20] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[6]);
	glBufferData(GL_ARRAY_BUFFER, bat.mPointCount * sizeof(vec3), &bat.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[6]);
	glBufferData(GL_ARRAY_BUFFER, bat.mPointCount * sizeof(vec3), &bat.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[6]);
	glBufferData(GL_ARRAY_BUFFER, bat.mPointCount * sizeof(vec2), &bat.mTextureCoords[0], GL_STATIC_DRAW);
	// Scene - terrain
	//loc1[21] = glGetAttribLocation(shaderProgramID, "vertex_position");
	//loc1[22] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	//loc1[23] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	//glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[7]);
	//glBufferData(GL_ARRAY_BUFFER, terrain.mPointCount * sizeof(vec3), &terrain.mNormals[0], GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[7]);
	//glBufferData(GL_ARRAY_BUFFER, terrain.mPointCount * sizeof(vec3), &terrain.mVertices[0], GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[7]);
	//glBufferData(GL_ARRAY_BUFFER, terrain.mPointCount * sizeof(vec2), &terrain.mTextureCoords[0], GL_STATIC_DRAW);
	// Scene - Castle
	loc2[0] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2[1] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc2[2] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[8]);
	glBufferData(GL_ARRAY_BUFFER, castle.mPointCount * sizeof(vec3), &castle.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[8]);
	glBufferData(GL_ARRAY_BUFFER, castle.mPointCount * sizeof(vec3), &castle.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[8]);
	glBufferData(GL_ARRAY_BUFFER, castle.mPointCount * sizeof(vec2), &castle.mTextureCoords[0], GL_STATIC_DRAW);
	// Scene - Ground
	loc2[3] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2[4] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc2[5] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[9]);
	glBufferData(GL_ARRAY_BUFFER, ground.mPointCount * sizeof(vec3), &ground.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[9]);
	glBufferData(GL_ARRAY_BUFFER, ground.mPointCount * sizeof(vec3), &ground.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[9]);
	glBufferData(GL_ARRAY_BUFFER, ground.mPointCount * sizeof(vec2), &ground.mTextureCoords[0], GL_STATIC_DRAW);
	// Scene - Bell
	loc2[6] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2[7] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc2[8] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[10]);
	glBufferData(GL_ARRAY_BUFFER, bell.mPointCount * sizeof(vec3), &bell.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[10]);
	glBufferData(GL_ARRAY_BUFFER, bell.mPointCount * sizeof(vec3), &bell.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[10]);
	glBufferData(GL_ARRAY_BUFFER, bell.mPointCount * sizeof(vec2), &bell.mTextureCoords[0], GL_STATIC_DRAW);
	// Scene - Lamp
	loc2[9] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2[10] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc2[11] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[11]);
	glBufferData(GL_ARRAY_BUFFER, lamp.mPointCount * sizeof(vec3), &lamp.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[11]);
	glBufferData(GL_ARRAY_BUFFER, lamp.mPointCount * sizeof(vec3), &lamp.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[11]);
	glBufferData(GL_ARRAY_BUFFER, lamp.mPointCount * sizeof(vec2), &lamp.mTextureCoords[0], GL_STATIC_DRAW);
	// Scene - Sky
	loc2[12] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2[13] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc2[14] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[12]);
	glBufferData(GL_ARRAY_BUFFER, sky.mPointCount * sizeof(vec3), &sky.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[12]);
	glBufferData(GL_ARRAY_BUFFER, sky.mPointCount * sizeof(vec3), &sky.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[12]);
	glBufferData(GL_ARRAY_BUFFER, sky.mPointCount * sizeof(vec2), &sky.mTextureCoords[0], GL_STATIC_DRAW);
	// Scene - Tower
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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Dark background
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

	// In display function, pass the spotlight properties to the shader
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
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 200.0f);

	// Update view and projection matrices in the shader
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, glm::value_ptr(projection));
	// Pass light to the shader
	glUniform3fv(lightPos_location, 1, glm::value_ptr(lightPos));
	glUniform3fv(viewPos_location, 1, glm::value_ptr(cameraPos));
	glUniform3fv(lightColor_location, 1, glm::value_ptr(lightColor));

	// Scene - Sea
	mat4 sea_model = identity_mat4();
	sea_model = rotate_x_deg(sea_model, 45.0f); // Apply rotation
	sea_model = rotate_y_deg(sea_model, 45.0f); // Apply rotation
	sea_model = translate(sea_model, vec3(0.0f, -0.2f, 0.0f));
	sea_model = scale(sea_model, vec3(100.0f, 100.0f, 100.0f));
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, sea_model.m);
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
	glBindTexture(GL_TEXTURE_2D, SEA_tex);
	glDrawArrays(GL_TRIANGLES, 0, sea.mPointCount);
	// Scene - Zombie
	mat4 zombie_model = identity_mat4();
	zombie_model = rotate_y_deg(zombie_model, zombieRotationAngle); // Apply rotation
	zombie_model = translate(zombie_model, vec3(zombiePosX, 0.5f, zombiePosZ));
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
	// Scene - Zombie 2
	mat4 zombie_model2 = identity_mat4();
	zombie_model2 = rotate_y_deg(zombie_model2, 90.0f); // Apply rotation
	zombie_model2 = rotate_y_deg(zombie_model2, zombie2RotationAngle); // Apply rotation
	zombie_model2 = translate(zombie_model2, vec3(zombie2PosX, 1.0f, zombie2PosZ));
	zombie_model2 = scale(zombie_model2, vec3(1.0f, 1.0f, 1.0f));
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
	// Scene - Skeleton
	mat4 human_model = identity_mat4();
	human_model = rotate_x_deg(human_model, 90.0f); // Apply rotation
	human_model = rotate_y_deg(human_model, 90.0f); // Apply rotation
	human_model = rotate_y_deg(human_model, humanRotationAngle); // Apply rotation
	human_model = translate(human_model, vec3(humanPosX, 0.5f, humanPosZ));
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, human_model.m);
	glEnableVertexAttribArray(loc1[9]);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[3]);
	glVertexAttribPointer(loc1[9], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc1[10]);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[3]);
	glVertexAttribPointer(loc1[10], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(VAOs[0]);
	glEnableVertexAttribArray(loc1[11]);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[3]);
	glVertexAttribPointer(loc1[11], 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindTexture(GL_TEXTURE_2D, HUMAN_tex);
	glDrawArrays(GL_TRIANGLES, 0, human.mPointCount);
	// Scene - Zombie 3
	vec3 zombie3Positions[] = {
		vec3(3.0f, 0.0f, -3.0f),  // Original position
		vec3(-3.0f, 0.0f, -3.0f),  // top left position
		vec3(3.0f, 0.0f,  3.0f),  // bottom right position
		vec3(-3.0f, 0.0f,  3.0f)   // bottom left position
	};
	float zombie3Rotations[] = {
		270,0,180,90
	};
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	float elapsedTime = (currentTime - startTime) / 1000.0f; // Elapsed time in seconds
	for (int i = 0; i < 4; ++i) {
		mat4 zombie_model3 = identity_mat4();
		zombie_model3 = rotate_y_deg(zombie_model3, zombie3Rotations[i]);
		zombie_model3 = translate(zombie_model3, zombie3Positions[i]);
		zombie_model3 = scale(zombie_model3, vec3(4.0f, 4.0f, 4.0f));
		float angle = elapsedTime * rotationSpeed;
		zombie_model3 = rotate_y_deg(zombie_model3, angle);
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, zombie_model3.m);
		glEnableVertexAttribArray(loc1[12]);
		glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[4]);
		glVertexAttribPointer(loc1[12], 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(loc1[13]);
		glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[4]);
		glVertexAttribPointer(loc1[13], 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindVertexArray(VAOs[0]);
		glEnableVertexAttribArray(loc1[14]);
		glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[4]);
		glVertexAttribPointer(loc1[14], 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindTexture(GL_TEXTURE_2D, ZOMBIE3_tex);
		glDrawArrays(GL_TRIANGLES, 0, zombie3.mPointCount);
	}
	// Scene - Monster
	mat4 monster_model = identity_mat4();
	monster_model = rotate_x_deg(monster_model, 90.0f); // Apply rotation
	monster_model = scale(monster_model, vec3(50.0f, 50.0f, 50.0f));
	monster_model = translate(monster_model, vec3(15.0f, 25.0f, -45.0f));
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, monster_model.m);
	glEnableVertexAttribArray(loc1[15]);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[5]);
	glVertexAttribPointer(loc1[15], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc1[16]);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[5]);
	glVertexAttribPointer(loc1[16], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(VAOs[0]);
	glEnableVertexAttribArray(loc1[17]);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[5]);
	glVertexAttribPointer(loc1[17], 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindTexture(GL_TEXTURE_2D, MONSTER_tex);
	glDrawArrays(GL_TRIANGLES, 0, monster.mPointCount);
	// Scene - Bat
	vec3 batPositions[] = {
		vec3(1.5f, 3.0f, -1.5f),    // top right position lower
		vec3(-1.5f, 3.0f, -1.5f),   // top left position lower
		vec3(1.5f, 3.0f,  1.5f),    // bottom right position lower
		vec3(-1.5f, 3.0f,  1.5f),	// bottom left position lower
		vec3(1.5f, 3.5f, 0.0f),	    // right position upper
		vec3(-1.5f, 3.5f, 0.0f),    // left position upper
		vec3(0.0f, 3.5f, 1.5f),     // top position upper
		vec3(0.0f, 3.5f, -1.5f)     // down position upper
	};
	float batRotations[] = {
		90,180,0,270,0,180,270,90
	};
	for (int i = 0; i < 8; ++i) {
		mat4 bat_model = identity_mat4();
		bat_model = rotate_y_deg(bat_model, batRotations[i]); // Apply rotation
		bat_model = translate(bat_model, batPositions[i]);
		bat_model = scale(bat_model, vec3(5.0f, 5.0f, 5.0f));
		float angle2 = elapsedTime * rotationSpeed * 10;
		bat_model = rotate_y_deg(bat_model, angle2);
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, bat_model.m);
		glEnableVertexAttribArray(loc1[18]);
		glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[6]);
		glVertexAttribPointer(loc1[18], 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(loc1[19]);
		glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[6]);
		glVertexAttribPointer(loc1[19], 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindVertexArray(VAOs[0]);
		glEnableVertexAttribArray(loc1[20]);
		glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[6]);
		glVertexAttribPointer(loc1[20], 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindTexture(GL_TEXTURE_2D, BAT_tex);
		glDrawArrays(GL_TRIANGLES, 0, bat.mPointCount);
	}
	// Terrain
	//mat4 terrainTransform = identity_mat4();
	//glUniformMatrix4fv(matrix_location, 1, GL_FALSE, terrainTransform.m);
	//glEnableVertexAttribArray(loc1[21]);
	//glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[7]);
	//glVertexAttribPointer(loc1[21], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glEnableVertexAttribArray(loc1[22]);
	//glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[7]);
	//glVertexAttribPointer(loc1[22], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glBindVertexArray(VAOs[0]);
	//glEnableVertexAttribArray(loc1[23]);
	//glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[7]);
	//glVertexAttribPointer(loc1[23], 2, GL_FLOAT, GL_FALSE, 0, NULL);
	//glBindTexture(GL_TEXTURE_2D, GRASS_tex);
	//glDrawArrays(GL_TRIANGLES, 0, terrain.mPointCount);
	// Scene - Castle
	mat4 castle_model = identity_mat4();
	castle_model = translate(castle_model, vec3(0.0f, -10.0f, 0.0f));
	castle_model = scale(castle_model, vec3(0.001f, 0.001f, 0.001f));
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, castle_model.m);
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
	glBindTexture(GL_TEXTURE_2D, CASTLE_tex);
	glDrawArrays(GL_TRIANGLES, 0, castle.mPointCount);
	// Scene - Ground
	mat4 gound_model = identity_mat4();
	gound_model = scale(gound_model, vec3(60.0f, 60.0f, 60.0f)); 
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, gound_model.m);
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
	// Scene - Bell
	mat4 bell_model = identity_mat4();
	bell_model = translate(bell_model, vec3(0.0f, 0.5f, 0.0f));
	bell_model = rotate_y_deg(bell_model, 180.0f); // Apply rotation
	bell_model = scale(bell_model, vec3(3.0f, 3.0f, 3.0f)); // Add the scaling transformation here
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, bell_model.m);
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
	// Scene - lamp
	vec3 lampPositions[] = {
		vec3(12.0f, 0.0f, -12.0f),  // Original position
		vec3(-12.0f, 0.0f, -12.0f),  // top left position
		vec3(12.0f, 0.0f,  12.0f),  // bottom right position
		vec3(-12.0f, 0.0f,  12.0f)   // bottom left position
	};

	for (int i = 0; i < 4; ++i) {
		mat4 light = identity_mat4();
		light = translate(light, lampPositions[i]);
		light = scale(light, vec3(0.2f, 0.2f, 0.2f));
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

	// Scene - Sky
	vec3 skyPositions[] = {
		vec3(0.0f, 0.0f, 0.5f), 
		vec3(0.0f, 0.0f, -0.5f), 
		vec3(0.0f, 0.5f, 0.0f),
		vec3(0.0f, -0.5f, 0.0f),
		vec3(0.5f, 0.0f, 0.0f),
		vec3(-0.5f, 0.0f, 0.0f),
	};
	float skyRotationsx[] = {
		90,90,0,0,0,0
	};
	float skyRotationsz[] = {
		0,0,0,0,90,90
	};
	for (int i = 0; i < 6; ++i) {
		mat4 sky_model = identity_mat4();
		sky_model = rotate_x_deg(sky_model, skyRotationsx[i]); // Apply rotation
		sky_model = rotate_z_deg(sky_model, skyRotationsz[i]); // Apply rotation
		sky_model = translate(sky_model, skyPositions[i]);
		sky_model = scale(sky_model, vec3(115.0f, 115.0f, 115.0f));
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, sky_model.m);
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
		glBindTexture(GL_TEXTURE_2D, SKY_tex);
		glDrawArrays(GL_TRIANGLES, 0, sky.mPointCount);
	}
	// Scene - Kennel
	// Array of positions for kennel instances
	vec3 towerPositions[] = {
		vec3(11.0f, 0.0f, -11.0f),  // Original position
		vec3(-11.0f, 0.0f, -11.0f),  // top left position
		vec3(9.0f, 0.0f,  11.5f),  // bottom right position
		vec3(-12.0f, 0.0f,  11.0f)   // bottom left position
	};

	float towerRotations[] = {
		0,0,180,180
	};

	for (int i = 0; i < 4; ++i) {
		mat4 stone_tower = identity_mat4();
		stone_tower = rotate_y_deg(stone_tower, towerRotations[i]); // Apply rotation
		stone_tower = translate(stone_tower, towerPositions[i]);
		stone_tower = scale(stone_tower, vec3(2.0f, 2.0f, 2.0f)); // Scale can be adjusted if needed
		glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, stone_tower.m);
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
		glBindTexture(GL_TEXTURE_2D, TOWER_tex);
		glDrawArrays(GL_TRIANGLES, 0, tower.mPointCount);
	}
	glutSwapBuffers();
}

void updateScene() {
	// Delta Calculation
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

	float lastPosX = humanPosX;
	float lastPosZ = humanPosZ;
	if (KeyUp) humanPosZ -= translationSpeed;
	if (KeyDown) humanPosZ += translationSpeed;
	if (KeyLeft) humanPosX -= translationSpeed;
	if (KeyRight) humanPosX += translationSpeed;
	// Define a threshold distance for when to stop chasing
	const float stopChasingDistance = 0.1f;
	// Calculate rotation angle
	if (humanPosX != lastPosX || humanPosZ != lastPosZ) {
		humanRotationAngle = atan2(lastPosZ - humanPosZ, humanPosX - lastPosX) * (180.0 / 3.1415926);
	}

	glm::vec3 humanPosition(humanPosX, 0.0f, humanPosZ);
	// first chasing
	glm::vec3 zombiePosition(zombiePosX, 0.0f, zombiePosZ);
	glm::vec3 directionToHumanZombie = glm::normalize(humanPosition - zombiePosition);
	if (glm::distance(zombiePosition, obstacle1Pos) < collisionDistance-0.5f) {
		// Alter path to avoid collision
		directionToHumanZombie = glm::vec3(-directionToHumanZombie.z, 0.0f, directionToHumanZombie.x);
	}
	if (glm::distance(zombiePosition, humanPosition) > stopChasingDistance) {
		zombiePosX += directionToHumanZombie.x * (movementSpeed - 0.002);
		zombiePosZ += directionToHumanZombie.z * (movementSpeed - 0.002);
	}
	zombieRotationAngle = atan2(-directionToHumanZombie.z, directionToHumanZombie.x) * (180.0 / 3.1415926);

	// second chasing
	glm::vec3 zombie2Position(zombie2PosX, 0.0f, zombie2PosZ);
	glm::vec3 directionToHumanZombie2 = glm::normalize(humanPosition - zombie2Position);
	// Check for collision with obstacles
	if (glm::distance(zombie2Position, obstacle1Pos) < collisionDistance-0.5f) {
		// Alter path to avoid collision
		directionToHumanZombie2 = glm::vec3(-directionToHumanZombie2.z, 0.0f, directionToHumanZombie2.x);
	}
	if (glm::distance(zombie2Position, humanPosition) > stopChasingDistance) {
		zombie2PosX += directionToHumanZombie2.x * (movementSpeed + 0.002);
		zombie2PosZ += directionToHumanZombie2.z * (movementSpeed + 0.002);
	}
	zombie2RotationAngle = atan2(-directionToHumanZombie2.z, directionToHumanZombie2.x) * (180.0 / 3.1415926);
	// Draw the next frame
	glutPostRedisplay();
}

void init()
{
	// Set up the shaders
	GLuint shaderProgramID = CompileShaders();
	// Load mesh into a vertex buffer array
	glutKeyboardFunc(keypress);
	glutSpecialFunc(specialKeyPress); // Register the special key press function
	glutSpecialUpFunc(specialKeyUp); // Register the special key up function
	glutKeyboardUpFunc(keyUp);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove); // Register mouse move callback
	generateObjectBufferMesh();
}



int main(int argc, char** argv) {
	startTime = glutGet(GLUT_ELAPSED_TIME); // Record the start time
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Apocalypse");
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutWarpPointer(width / 2, height / 2);
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	init();
	//PlaySound(TEXT(AMBIENT), NULL, SND_ASYNC);
	glutMainLoop();
	return 0;
}
//-----------------------------------------------------------------------------------------------------------
//float lerp(float a, float b, float t) {
//    return a + t * (b - a);
//}
//
//float fade(float t) {
//    // 6t^5 - 15t^4 + 10t^3
//    return t * t * t * (t * (t * 6 - 15) + 10);
//}
//
//float grad(int hash, float x, float y, float z) {
//    int h = hash & 15;
//    float u = h < 8 ? x : y;
//    float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
//    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
//}
//int permutation[256] = { 23 }; //Your permutation values go here
//float perlinNoise(float x, float y, float z) {
//    int X = (int)floor(x) & 255;
//    int Y = (int)floor(y) & 255;
//    int Z = (int)floor(z) & 255;
//
//    x -= floor(x);
//    y -= floor(y);
//    z -= floor(z);
//
//    float u = fade(x);
//    float v = fade(y);
//    float w = fade(z);
//
//    int p[512];
//    for (int i = 0; i < 256; i++) p[256 + i] = p[i] = permutation[i];
//
//    Ensure that A, B, AA, AB, BA, and BB are within the array bounds
//    int A = p[X & 255] + Y;
//    int B = p[(X + 1) & 255] + Y;
//    int AA = p[A & 255] + Z;
//    int AB = p[(A + 1) & 255] + Z;
//    int BA = p[B & 255] + Z;
//    int BB = p[(B + 1) & 255] + Z;
//
//    return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z), grad(p[BA], x - 1, y, z)), lerp(u, grad(p[AB], x, y - 1, z), grad(p[BB], x - 1, y - 1, z))), lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1), grad(p[BA + 1], x - 1, y, z - 1)), lerp(u, grad(p[AB + 1], x, y - 1, z - 1), grad(p[BB + 1], x - 1, y - 1, z - 1))));
//}
//ModelData generateTerrain(int gridX, int gridZ, float gridSize) {
//    ModelData terrain;
//    int vertexCount = gridX * gridZ;
//    terrain.mPointCount = 6 * (gridX - 1) * (gridZ - 1);
//
//    for (int i = 0; i < gridZ; i++) {
//        for (int j = 0; j < gridX; j++) {
//            float x = j * gridSize;
//            float z = i * gridSize;
//            float y = perlinNoise(x * 0.1, 0.0f, z * 0.1); // Adjust x and z scaling factors as needed
//
//            terrain.mVertices.push_back(vec3(x, y, z));
//            terrain.mNormals.push_back(vec3(0, 1, 0)); // Placeholder normal, calculate based on surrounding vertices for better results.
//            terrain.mTextureCoords.push_back(vec2((float)j / (gridX - 1), (float)i / (gridZ - 1)));
//        }
//    }
//    for (int gz = 0; gz < gridZ - 1; gz++) {
//        for (int gx = 0; gx < gridX - 1; gx++) {
//            int topLeft = (gz * gridX) + gx;
//            int topRight = topLeft + 1;
//            int bottomLeft = ((gz + 1) * gridX) + gx;
//            int bottomRight = bottomLeft + 1;
//
//            // Triangle 1
//            terrain.mIndices.push_back(topLeft);
//            terrain.mIndices.push_back(bottomLeft);
//            terrain.mIndices.push_back(topRight);
//
//            // Triangle 2
//            terrain.mIndices.push_back(topRight);
//            terrain.mIndices.push_back(bottomLeft);
//            terrain.mIndices.push_back(bottomRight);
//        }
//    }
//
//    return terrain;
//}



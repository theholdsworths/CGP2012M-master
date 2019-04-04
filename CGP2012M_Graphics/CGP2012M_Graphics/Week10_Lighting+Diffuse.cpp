//#include <cstdio>
//#include <cstdlib>
#include <iostream>
#include <vector>

//include shape, shader header files
#include "GLerror.h"
#include "SDL_Start.h"
#include "Triangle_T.h"
#include "Circle.h"
#include "CircleTexture.h"
#include "Square.h"
#include "Camera.h"
#include "Cube.h"
#include "Model.h"
#include "ModelLoaderClass.h"
#ifndef TEXTURECLASS_H
#define TEXTURECLASS_H
#ifndef SHADERCLASS_H
#define SHADERCLASS_H

// // GLEW - OpenGL Extension Wrangler - http://glew.sourceforge.net/
// // NOTE: include before SDL.h
#ifndef GLEW_H
#define GLEW_H
//#include <GL/glew.h>
#include "windows.h"

// SDL - Simple DirectMedia Layer - https://www.libsdl.org/
#ifndef SDL_H
#define SDL_H
#include "SDL.h"
#include "SDL_image.h"
//#include "SDL_mixer.h"
//#include "SDL_ttf.h"

// // - OpenGL Mathematics - https://glm.g-truc.net/
#define GLM_FORCE_RADIANS // force glm to use radians
// // NOTE: must do before including GLM headers
// // NOTE: GLSL uses radians, so will do the same, for consistency
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//***************
//variables
void Uniform_variables();
SDL_Event event;
SDL_Window *win;
bool windowOpen = true;
bool isFullScreen = false;

bool SpawnBullet = false;
bool shot[10];

float PlayerX = 0.f, PlayerY = -0.45f, PlayerZ = 2.0f, BulletX = 2.0f, BulletY = 0.2f;
float bubbleSpeed = -0.001f;
float radius;
//screen boundaries for collision tests
float bX_r = 2.0f;
float bX_l = -2.0f;
float bY_t = 1.0f;
float bY_b = -1.0f;
//screen centre
float centreX = 0.0f;
float centreY = 0.0f;
//window aspect/width/height
int w;
int h;
float aspect;
int left;
int newwidth;
int newheight;

bool MoveRight = false, MoveLeft = false;

//transform matrices
glm::mat4 modelMatrix;
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;
glm::mat4 normalMatrix;

glm::mat4 translate;
glm::mat4 rotate;
glm::mat4 scale;
glm::mat4 backgroundTranslate;
glm::mat4 backgroundScale;
glm::vec3 b_scaleFactor;
glm::mat4 modelRotate;
glm::mat4 modelScale;
glm::mat4 modelTranslate;
//cube
glm::mat4 EarthRotate;
glm::mat4 EarthScale;
glm::mat4 EarthTranslate;
//border-Right
glm::mat4 RBorderRotate;
glm::mat4 RBorderScale;
glm::mat4 RBorderTranslate;
//border-left
glm::mat4 LBorderRotate;
glm::mat4 LBorderScale;
glm::mat4 LBorderTranslate;
//block
glm::mat4 BlockRotate;
glm::mat4 BlockScale;
glm::mat4 BlockTranslate;

glm::mat4 Block2Translate;
glm::mat4 Block3Translate;
glm::mat4 Block4Translate;
glm::mat4 Block5Translate;
glm::mat4 Block6Translate;
float angle = 0;

//create camera
Camera cam;
//loac camera variables
glm::vec3 camPos;
glm::vec3 camTarget;

bool flag = true;

glm::vec3 lightCol;
glm::vec3 lightPosition;
glm::vec3 viewPosition;
float ambientIntensity;

glm::vec3 lightCol2;
glm::vec3 lightPosition2;
glm::vec3 viewPosition2;

//**************
//function prototypes
CircleTexture updatePositions(CircleTexture c);
void handleInput();
//void AspectRatio

int main(int argc, char *argv[]) {
	//start and initialise SDL
	SDL_Start sdl;
	SDL_GLContext context = sdl.Init();
	win = sdl.win;

	SDL_GetWindowSize(win, &w, &h);
	glViewport(0, 0, w, h);
	aspect = (float)w / (float)h;

	//error class
	GLerror glerr;
	int errorLabel;

	//GLEW initialise
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	//register debug callback
	if (glDebugMessageCallback)
	{
		std::cout << "Registering OpenGL Debug callback function" << std::endl;
		glDebugMessageCallback(glerr.openglCallbackFunction, &errorLabel);
		glDebugMessageControl(GL_DONT_CARE,
			GL_DONT_CARE,
			GL_DONT_CARE,
			0,
			NULL,
			true);
	}

	//*****************************************************
	//OpenGL specific data

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//objects
	//create background square
	Square background;
	//create model
	//could make this better by specifying the texture in this model header
	Model model;
	Model cube;
	Model blocks[10];
	//create model loader
	ModelImport modelLoader; 
	modelLoader.LoadOBJ2("..//..//Assets//Models//blenderSphere.obj", model.vertices, model.texCoords, model.normals, model.indices);
	modelLoader.LoadOBJ2("..//..//Assets//Models//blenderCube.obj", cube.vertices, cube.texCoords, cube.normals, cube.indices);

	errorLabel = 0;

	//*********************
	//create texture collection
	//create textures - space for 4, but only using 2
	Texture texArray[8];
	//background texture
	texArray[0].load("..//..//Assets//Textures//space.png");
	texArray[0].setBuffers();
	texArray[1].load("..//..//Assets//Textures//bubble.png");
	texArray[1].setBuffers();
	texArray[2].load("..//..//Assets//Textures//bricks.png");
	texArray[2].setBuffers();
	texArray[3].load("..//..//Assets//Textures//earthmap1k.png");
	texArray[3].setBuffers();

	errorLabel = 3;

	//OpenGL buffers
	background.setBuffers();
	model.setBuffers();
	cube.setBuffers();

	errorLabel = 3;
	//*****************************************
	//set uniform variables
	int transformLocation;
	int modelLocation;
	int viewLocation;
	int projectionLocation;

	int importModelLocation, importViewLocation, importProjectionLocation;
	int earthModelLocation, earthViewLocation, earthProjectionLocation;
	int borderModeLocation, borderViewLocation, borderProjectionLocation;
	int blockModeLocation, blockViewLocation, blockProjectionLocation;
	
	
	int backgroundColourLocation;
	int ambientIntensityLocation;
	int modelColourLocation;
	int modelAmbientLocation;

	int lightColLocation;
	int normalMatrixLocation;
	int lightPositionLocation;
	int viewPositionLocation;

	int lightCol2Location;
	int lightPosition2Location;
	int viewPosition2Location;

	GLuint currentTime = 0;
	GLuint lastTime = 0;
	GLuint elapsedTime = 0;

	//lighting for the model
	//Light position setting
	lightPosition = glm::vec3(1.0f, 0.0f, 0.5f);
	lightPosition2 = glm::vec3(2.0f, 0.0f, 1.5f);
	//light colour setting
	// Candle:  r:1.0 g:0.57 b:0.16
	// 100W bulb: r:1.0 g:0.84 b:0.66
	// direct sunlight: r:1.0 g:1.0 b:0.98
	glm::vec3 lightColour = glm::vec3(1.0f, 1.f, 0.95f);
	glm::vec3 lightColour2 = glm::vec3(1.0f, 0.84f, 0.66f);

	//light for the background
	//light distance setting
	ambientIntensity = 1.0f;
	lightCol = glm::vec3(1.0f, 1.0f, 0.98f);
	lightCol2 = glm::vec3(0.7f, 0.0f, 0.98f);

	//initialise transform matrices 
	//orthographic (2D) projection
	//projectionMatrix = glm::ortho(0.0f, 4.0f, 0.0f, 3.0f, -1.0f, 100.0f);
	//perspective (3D) projection
	projectionMatrix = glm::perspective(glm::radians(45.0f), (float)w / (float)h, 0.1f, 100.0f);
	//initialise view matrix to '1'
	viewMatrix = glm::mat4(1.0f);

	/*for (int i = 0; i < 10; i++)
		blocks[i].setBuffers();*/

	backgroundScale = glm::mat4(1.0f);
	backgroundTranslate = glm::mat4(1.0f);
	modelScale = glm::mat4(4.0f);
	modelRotate = glm::mat4(1.0f);
	modelTranslate = glm::mat4(1.0f);
	//Cube
	EarthScale = glm::mat4(1.0f);
	EarthRotate = glm::mat4(1.0f);
	EarthTranslate = glm::mat4(1.0f);
	//border
	RBorderScale = glm::mat4(1.0f);
	RBorderRotate = glm::mat4(1.0f);
	RBorderTranslate = glm::mat4(1.0f);
	//Left border 
	LBorderScale = glm::mat4(1.0f);
	LBorderRotate = glm::mat4(1.0f);
	LBorderTranslate = glm::mat4(1.0f);
	//block
	BlockScale = glm::mat4(1.0f);
	BlockRotate = glm::mat4(1.0f);
	BlockTranslate = glm::mat4(1.0f);

	Block2Translate = glm::mat4(1.0f);
	Block3Translate = glm::mat4(1.0f);
	Block4Translate = glm::mat4(1.0f);
	Block5Translate = glm::mat4(1.0f);
	Block6Translate = glm::mat4(1.0f);

	//once only scale to background, and translate to centre
	b_scaleFactor = { 80.0f, 70.0f, 1.0f };
	backgroundScale = glm::scale(backgroundScale, glm::vec3(b_scaleFactor));
	backgroundTranslate = glm::translate(backgroundTranslate, glm::vec3(0.0f, 0.0f, -10.0f));

	//once only scale and translate to model
	modelScale = glm::scale(modelScale, glm::vec3(0.07f, 0.07f, 0.07f));
	modelTranslate = glm::translate(modelTranslate, glm::vec3(0.0f, -0.3f, 2.0f));

	EarthScale = glm::scale(EarthScale, glm::vec3(0.4f, 0.05f, 0.05f));
	EarthTranslate = glm::translate(EarthTranslate, glm::vec3(PlayerX, PlayerY, PlayerZ));

	RBorderScale = glm::scale(RBorderScale, glm::vec3(0.1f, 1.7f, 4.7f));
	RBorderTranslate = glm::translate(RBorderTranslate, glm::vec3(4.0f, 0.0f, -4.7f));

	LBorderScale = glm::scale(LBorderScale, glm::vec3(0.1f, 1.7f, 4.7f));
	LBorderTranslate = glm::translate(LBorderTranslate, glm::vec3(-3.0, 0.0f, -4.7));

	BlockScale = glm::scale(BlockScale, glm::vec3(0.2f, 0.07f, 0.07f));
	BlockTranslate = glm::translate(BlockTranslate, glm::vec3(-.5f, .5f, 2.0f));	
	Block2Translate = glm::translate(Block2Translate, glm::vec3(0.0f, .5f, 2.0f));
	Block3Translate = glm::translate(Block3Translate, glm::vec3(0.5, .5f, 2.0f));

	Block4Translate = glm::translate(Block3Translate, glm::vec3(-.5, .0f, 2.0f));
	Block5Translate = glm::translate(Block3Translate, glm::vec3(0.0, .0f, 2.0f));
	Block6Translate = glm::translate(Block3Translate, glm::vec3(0.5, .0f, 2.0f));

	errorLabel = 4;

	//*****************************
	//'game' loop
	while (windowOpen)
	{

		if (MoveRight == true)
		{
			if (PlayerX <= 10.f)
			{
				PlayerX += 0.05f;
			}
			else
			{
				MoveRight = false;
			}
		}
		if (MoveLeft == true)
		{
			if (PlayerX-= -10.35f)
			{
				PlayerX -= 0.05f;
			}
			else
			{
				MoveLeft = false;
			}
		}
		//*************************
		//****************************
		// OpenGL calls.
		glClearColor(1.0f, 1.0f, 1.0f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//process inputs
		handleInput();
		cam.updateCamera();

		//time
		currentTime = SDL_GetTicks();
		elapsedTime = currentTime - lastTime;
		lastTime = currentTime;

		//update camera matrix
		//camera only moves side to side, formards and backwards (no rotation)
		// set position, target, up direction
		viewMatrix = glm::lookAt(glm::vec3(cam.camXPos, cam.camYPos, cam.camZPos), cam.cameraTarget, cam.cameraUp);

		errorLabel = 5;

		//background
		glUseProgram(background.shaderProgram);
		//set background lighting
		backgroundColourLocation = glGetUniformLocation(background.shaderProgram, "uLightColour");
		glProgramUniform3fv(background.shaderProgram, backgroundColourLocation, 1, glm::value_ptr(lightCol));
		//light distance
		ambientIntensityLocation = glGetUniformLocation(background.shaderProgram, "uAmbientIntensity");
		glProgramUniform1f(background.shaderProgram, ambientIntensityLocation, ambientIntensity);

		//set background image
		modelLocation = glGetUniformLocation(background.shaderProgram, "uModel");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(backgroundTranslate*backgroundScale));
		viewLocation = glGetUniformLocation(background.shaderProgram, "uView");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		projectionLocation = glGetUniformLocation(background.shaderProgram, "uProjection");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glBindTexture(GL_TEXTURE_2D, texArray[0].texture);
		background.render();

		////set earth
		glUseProgram(model.shaderProgram);
		//lighting uniforms
		//get and set light colour and position uniform
		lightColLocation = glGetUniformLocation(cube.shaderProgram, "lightCol");
		glUniform3fv(lightColLocation, 1, glm::value_ptr(lightColour));
		lightPositionLocation = glGetUniformLocation(model.shaderProgram, "lightPos");
		glUniform3fv(lightPositionLocation, 1, glm::value_ptr(lightPosition));
		//rotation
		//EarthRotate = glm::rotate(EarthRotate, (float)elapsedTime / 2000, glm::vec3(0.0f, 1.0f, 0.0f));
		importModelLocation = glGetUniformLocation(model.shaderProgram, "uModel");
		glUniformMatrix4fv(importModelLocation, 1, GL_FALSE, glm::value_ptr(EarthTranslate*EarthRotate*EarthScale));
		importViewLocation = glGetUniformLocation(cube.shaderProgram, "uView");
		glUniformMatrix4fv(importViewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		importProjectionLocation = glGetUniformLocation(cube.shaderProgram, "uProjection");
		glUniformMatrix4fv(importProjectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		//set the normal matrix to send to the vertex shader, Light calculations take place in model-view space, So we calculate the normal matrix in that space
		normalMatrix = glm::transpose(glm::inverse(EarthTranslate*EarthRotate*EarthScale * viewMatrix));
		//set the normalMatrix in the shaders
		glUseProgram(model.shaderProgram);
		normalMatrixLocation = glGetUniformLocation(cube.shaderProgram, "uNormalMatrix");
		glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		glBindTexture(GL_TEXTURE_2D, texArray[3].texture);
		cube.render();

		borderModeLocation, borderViewLocation, borderProjectionLocation;
		////set border
		glUseProgram(cube.shaderProgram);
		////lighting uniforms
		////get and set light colour and position uniform
		lightColLocation = glGetUniformLocation(cube.shaderProgram, "lightCol");
		glUniform3fv(lightColLocation, 1, glm::value_ptr(lightColour));
		lightPositionLocation = glGetUniformLocation(cube.shaderProgram, "lightPosition");
		glUniform3fv(lightPositionLocation, 1, glm::value_ptr(lightPosition));
		////rotation
		//cubeRotate = glm::rotate(cubeRotate, (float)elapsedTime / 1000, glm::vec3(1.0f, 1.0f, 0.0f));
		borderModeLocation = glGetUniformLocation(cube.shaderProgram, "uModel");
		glUniformMatrix4fv(borderModeLocation, 1, GL_FALSE, glm::value_ptr(RBorderTranslate*RBorderRotate*RBorderScale));
		borderViewLocation = glGetUniformLocation(cube.shaderProgram, "uView");
		glUniformMatrix4fv(borderViewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		borderProjectionLocation = glGetUniformLocation(cube.shaderProgram, "uProjection");
		glUniformMatrix4fv(borderProjectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		////set the normal matrix to send to the vertex shader
		normalMatrix = (glm::mat3)glm::transpose(glm::inverse(RBorderTranslate*RBorderRotate*RBorderScale));
		////set the normalMatrix in the shaders
		normalMatrixLocation = glGetUniformLocation(cube.shaderProgram, "uNormalMatrix");
		glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		glBindTexture(GL_TEXTURE_2D, texArray[2].texture);
		cube.render();

		//set border
		glUseProgram(cube.shaderProgram);
		////lighting uniforms get and set light colour and position uniform
		lightColLocation = glGetUniformLocation(cube.shaderProgram, "lightCol");
		glUniform3fv(lightColLocation, 1, glm::value_ptr(lightColour));
		lightPositionLocation = glGetUniformLocation(cube.shaderProgram, "lightPosition");
		glUniform3fv(lightPositionLocation, 1, glm::value_ptr(lightPosition));
		//rotation // cubeRotate = glm::rotate(cubeRotate, (float)elapsedTime / 1000, glm::vec3(1.0f, 1.0f, 0.0f));
		borderModeLocation = glGetUniformLocation(cube.shaderProgram, "uModel");
		glUniformMatrix4fv(borderModeLocation, 1, GL_FALSE, glm::value_ptr(LBorderTranslate*LBorderRotate*LBorderScale));
		borderViewLocation = glGetUniformLocation(cube.shaderProgram, "uView");
		glUniformMatrix4fv(borderViewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		borderProjectionLocation = glGetUniformLocation(cube.shaderProgram, "uProjection");
		glUniformMatrix4fv(borderProjectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		//set the normal matrix to send to the vertex shader
		normalMatrix = (glm::mat3)glm::transpose(glm::inverse(LBorderTranslate*LBorderRotate*LBorderScale));
		//set the normalMatrix in the shaders
		normalMatrixLocation = glGetUniformLocation(cube.shaderProgram, "uNormalMatrix");
		glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		glBindTexture(GL_TEXTURE_2D, texArray[2].texture);
		cube.render();

		//set block
		glUseProgram(cube.shaderProgram);
		////lighting uniforms get and set light colour and position uniform
		lightColLocation = glGetUniformLocation(cube.shaderProgram, "lightCol");
		glUniform3fv(lightColLocation, 1, glm::value_ptr(lightColour));
		lightPositionLocation = glGetUniformLocation(cube.shaderProgram, "lightPosition");
		glUniform3fv(lightPositionLocation, 1, glm::value_ptr(lightPosition));
		//rotation // cubeRotate = glm::rotate(cubeRotate, (float)elapsedTime / 1000, glm::vec3(1.0f, 1.0f, 0.0f));
		blockModeLocation = glGetUniformLocation(cube.shaderProgram, "uModel");
		glUniformMatrix4fv(blockModeLocation, 1, GL_FALSE, glm::value_ptr(BlockTranslate*BlockRotate*BlockScale));
		blockViewLocation = glGetUniformLocation(cube.shaderProgram, "uView");
		glUniformMatrix4fv(blockViewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		blockProjectionLocation = glGetUniformLocation(cube.shaderProgram, "uProjection");
		glUniformMatrix4fv(blockProjectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		//set the normal matrix to send to the vertex shader
		normalMatrix = (glm::mat3)glm::transpose(glm::inverse(BlockTranslate*BlockRotate*BlockScale));
		//set the normalMatrix in the shaders
		normalMatrixLocation = glGetUniformLocation(cube.shaderProgram, "uNormalMatrix");
		glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		glBindTexture(GL_TEXTURE_2D, texArray[2].texture);
		cube.render();
		
		//set block
		glUseProgram(cube.shaderProgram);
		////lighting uniforms get and set light colour and position uniform
		lightColLocation = glGetUniformLocation(cube.shaderProgram, "lightCol");
		glUniform3fv(lightColLocation, 1, glm::value_ptr(lightColour));
		lightPositionLocation = glGetUniformLocation(cube.shaderProgram, "lightPosition");
		glUniform3fv(lightPositionLocation, 1, glm::value_ptr(lightPosition));
		//rotation // cubeRotate = glm::rotate(cubeRotate, (float)elapsedTime / 1000, glm::vec3(1.0f, 1.0f, 0.0f));
		blockModeLocation = glGetUniformLocation(cube.shaderProgram, "uModel");
		glUniformMatrix4fv(blockModeLocation, 1, GL_FALSE, glm::value_ptr(Block2Translate*BlockRotate*BlockScale));
		blockViewLocation = glGetUniformLocation(cube.shaderProgram, "uView");
		glUniformMatrix4fv(blockViewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		blockProjectionLocation = glGetUniformLocation(cube.shaderProgram, "uProjection");
		glUniformMatrix4fv(blockProjectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		//set the normal matrix to send to the vertex shader
		normalMatrix = (glm::mat3)glm::transpose(glm::inverse(Block2Translate*BlockRotate*BlockScale));
		//set the normalMatrix in the shaders
		normalMatrixLocation = glGetUniformLocation(cube.shaderProgram, "uNormalMatrix");
		glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		glBindTexture(GL_TEXTURE_2D, texArray[2].texture);
		cube.render();

		//set block
		glUseProgram(cube.shaderProgram);
		////lighting uniforms get and set light colour and position uniform
		lightColLocation = glGetUniformLocation(cube.shaderProgram, "lightCol");
		glUniform3fv(lightColLocation, 1, glm::value_ptr(lightColour));
		lightPositionLocation = glGetUniformLocation(cube.shaderProgram, "lightPosition");
		glUniform3fv(lightPositionLocation, 1, glm::value_ptr(lightPosition));
		//rotation // cubeRotate = glm::rotate(cubeRotate, (float)elapsedTime / 1000, glm::vec3(1.0f, 1.0f, 0.0f));
		blockModeLocation = glGetUniformLocation(cube.shaderProgram, "uModel");
		glUniformMatrix4fv(blockModeLocation, 1, GL_FALSE, glm::value_ptr(Block3Translate*BlockRotate*BlockScale));
		blockViewLocation = glGetUniformLocation(cube.shaderProgram, "uView");
		glUniformMatrix4fv(blockViewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		blockProjectionLocation = glGetUniformLocation(cube.shaderProgram, "uProjection");
		glUniformMatrix4fv(blockProjectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		//set the normal matrix to send to the vertex shader
		normalMatrix = (glm::mat3)glm::transpose(glm::inverse(Block3Translate*BlockRotate*BlockScale));
		//set the normalMatrix in the shaders
		normalMatrixLocation = glGetUniformLocation(cube.shaderProgram, "uNormalMatrix");
		glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		glBindTexture(GL_TEXTURE_2D, texArray[2].texture);
		cube.render();

		//set block
		glUseProgram(cube.shaderProgram);
		////lighting uniforms get and set light colour and position uniform
		lightColLocation = glGetUniformLocation(cube.shaderProgram, "lightCol");
		glUniform3fv(lightColLocation, 1, glm::value_ptr(lightColour));
		lightPositionLocation = glGetUniformLocation(cube.shaderProgram, "lightPosition");
		glUniform3fv(lightPositionLocation, 1, glm::value_ptr(lightPosition));
		//rotation // cubeRotate = glm::rotate(cubeRotate, (float)elapsedTime / 1000, glm::vec3(1.0f, 1.0f, 0.0f));
		blockModeLocation = glGetUniformLocation(cube.shaderProgram, "uModel");
		glUniformMatrix4fv(blockModeLocation, 1, GL_FALSE, glm::value_ptr(Block4Translate*BlockRotate*BlockScale));
		blockViewLocation = glGetUniformLocation(cube.shaderProgram, "uView");
		glUniformMatrix4fv(blockViewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		blockProjectionLocation = glGetUniformLocation(cube.shaderProgram, "uProjection");
		glUniformMatrix4fv(blockProjectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		//set the normal matrix to send to the vertex shader
		normalMatrix = (glm::mat3)glm::transpose(glm::inverse(Block4Translate*BlockRotate*BlockScale));
		//set the normalMatrix in the shaders
		normalMatrixLocation = glGetUniformLocation(cube.shaderProgram, "uNormalMatrix");
		glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		glBindTexture(GL_TEXTURE_2D, texArray[2].texture);
		cube.render();

		//BLOCK 4
		glUseProgram(cube.shaderProgram);
		////lighting uniforms get and set light colour and position uniform
		lightColLocation = glGetUniformLocation(cube.shaderProgram, "lightCol");
		glUniform3fv(lightColLocation, 1, glm::value_ptr(lightColour));
		lightPositionLocation = glGetUniformLocation(cube.shaderProgram, "lightPosition");
		glUniform3fv(lightPositionLocation, 1, glm::value_ptr(lightPosition));
		//rotation // cubeRotate = glm::rotate(cubeRotate, (float)elapsedTime / 1000, glm::vec3(1.0f, 1.0f, 0.0f));
		blockModeLocation = glGetUniformLocation(cube.shaderProgram, "uModel");
		glUniformMatrix4fv(blockModeLocation, 1, GL_FALSE, glm::value_ptr(Block5Translate*BlockRotate*BlockScale));
		blockViewLocation = glGetUniformLocation(cube.shaderProgram, "uView");
		glUniformMatrix4fv(blockViewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		blockProjectionLocation = glGetUniformLocation(cube.shaderProgram, "uProjection");
		glUniformMatrix4fv(blockProjectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		//set the normal matrix to send to the vertex shader
		normalMatrix = (glm::mat3)glm::transpose(glm::inverse(Block5Translate*BlockRotate*BlockScale));
		//set the normalMatrix in the shaders
		normalMatrixLocation = glGetUniformLocation(cube.shaderProgram, "uNormalMatrix");
		glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		glBindTexture(GL_TEXTURE_2D, texArray[2].texture);
		cube.render();
		//for (int x = 0; x < 10; x++)
		//{
		//	//set border
		//	glUseProgram(cube.shaderProgram);
		//	////lighting uniforms get and set light colour and position uniform
		//	lightColLocation = glGetUniformLocation(cube.shaderProgram, "lightCol");
		//	glUniform3fv(lightColLocation, 1, glm::value_ptr(lightColour));
		//	lightPositionLocation = glGetUniformLocation(cube.shaderProgram, "lightPosition");
		//	glUniform3fv(lightPositionLocation, 1, glm::value_ptr(lightPosition));
		//	//rotation // cubeRotate = glm::rotate(cubeRotate, (float)elapsedTime / 1000, glm::vec3(1.0f, 1.0f, 0.0f));
		//	borderModeLocation = glGetUniformLocation(cube.shaderProgram, "uModel");
		//	glUniformMatrix4fv(borderModeLocation, 1, GL_FALSE, glm::value_ptr(LBorderTranslate*LBorderRotate*LBorderScale));
		//	borderViewLocation = glGetUniformLocation(cube.shaderProgram, "uView");
		//	glUniformMatrix4fv(borderViewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		//	borderProjectionLocation = glGetUniformLocation(cube.shaderProgram, "uProjection");
		//	glUniformMatrix4fv(borderProjectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		//	//set the normal matrix to send to the vertex shader
		//	normalMatrix = (glm::mat3)glm::transpose(glm::inverse(LBorderTranslate*LBorderRotate*LBorderScale));
		//	//set the normalMatrix in the shaders
		//	normalMatrixLocation = glGetUniformLocation(cube.shaderProgram, "uNormalMatrix");
		//	glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		//	glBindTexture(GL_TEXTURE_2D, texArray[2].texture);
		//	cube.render();

		//}
		
		if (SpawnBullet == true)
		{
			////set .obj model
			glUseProgram(model.shaderProgram);
			//lighting uniforms
			//get and set light colour and position uniform
			lightColLocation = glGetUniformLocation(model.shaderProgram, "lightCol");
			glUniform3fv(lightColLocation, 1, glm::value_ptr(lightColour));
			lightPositionLocation = glGetUniformLocation(model.shaderProgram, "lightPos");
			glUniform3fv(lightPositionLocation, 1, glm::value_ptr(lightPosition));
			//rotation
			modelRotate = glm::rotate(modelRotate, (float)elapsedTime / 2000, glm::vec3(0.0f, 1.0f, 0.0f));
			importModelLocation = glGetUniformLocation(model.shaderProgram, "uModel");
			glUniformMatrix4fv(importModelLocation, 1, GL_FALSE, glm::value_ptr(modelTranslate*modelRotate*modelScale));
			importViewLocation = glGetUniformLocation(model.shaderProgram, "uView");
			glUniformMatrix4fv(importViewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			importProjectionLocation = glGetUniformLocation(model.shaderProgram, "uProjection");
			glUniformMatrix4fv(importProjectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			//set the normal matrix to send to the vertex shader
			//Light calculations take place in model-view space
			//So we calculate the normal matrix in that space
			normalMatrix = glm::transpose(glm::inverse(modelTranslate*modelRotate*modelScale * viewMatrix));
			//set the normalMatrix in the shaders
			glUseProgram(model.shaderProgram);
			normalMatrixLocation = glGetUniformLocation(model.shaderProgram, "uNormalMatrix");
			glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));
			glBindTexture(GL_TEXTURE_2D, texArray[1].texture);
			model.render();

			while (SpawnBullet == true)
			{
				BulletY += 0.05f;
				if (BulletY >= 20.5f)
				{
					SpawnBullet = false;
					BulletY = PlayerY;
					break;
				}
				break;
			}
		}

		//set to wireframe so we can see the circles
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		SDL_GL_SwapWindow(sdl.win);

	}//end loop

	//****************************
	// Once finished with OpenGL functions, the SDL_GLContext can be deleted.
	SDL_GL_DeleteContext(context);

	SDL_Quit();
	return 0;
}

void handleInput()
{
	//*****************************
		//SDL handled input
		//Any input to the program is done here

	if (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			windowOpen = false;
		}
		if (event.type == SDL_WINDOWEVENT)
		{
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				std::cout << "Window resized w:" << w << " h:" << h << std::endl;
				SDL_GetWindowSize(win, &w, &h);
				newwidth = h * aspect;
				left = (w - newwidth) / 2;
				glViewport(left, 0, newwidth, h);
				break;
			case SDL_WINDOWEVENT: 
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				break;

			default:
				break;
			}
		}
		
		if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				SDL_Quit();
				break;			
			case SDLK_UP:
				translate = glm::translate(translate, glm::vec3((float)cos(angle)*0.02f, (float)sin(angle)*0.02f, 0.0f));
				break;
			case SDLK_LEFT:
				angle += glm::radians(10.0f);
				rotate = glm::rotate(rotate,glm::radians(10.0f), glm::vec3(0, 0, 1));
				break;
			case SDLK_RIGHT:
				angle -= glm::radians(10.0f);
				rotate = glm::rotate(rotate, glm::radians(-10.0f) , glm::vec3(0, 0, 1));
				break;
			case SDLK_p:
				//move camera 'forward' in the -ve z direction
				cam.camZPos -= cam.camSpeed;
				break;
			case SDLK_l:
				//move camera 'backwards' in +ve z direction
				cam.camZPos += cam.camSpeed;
				break;
			case SDLK_z:
				//move camera left
				//move camera target with position
				cam.camXPos -= cam.camSpeed;
				cam.camXTarget -= cam.camSpeed;
				break;
			case SDLK_x:
				//move camera right
				//move camera target with position
				cam.camXPos += cam.camSpeed;
				cam.camXTarget += cam.camSpeed;
				break;

			case SDLK_m:
				//move camera up
				cam.camYPos += cam.camSpeed;
				cam.camYTarget += cam.camSpeed;
				break;
			case SDLK_k:
				//move camera down
				cam.camYPos -= cam.camSpeed;
				cam.camYTarget -= cam.camSpeed;
				break;
			case SDLK_v:
				lightPosition.x -= 0.1f;
				break;
			case SDLK_b:
				lightPosition.x += 0.1f;
				break;
			case SDLK_a:
				MoveLeft = true;
				//modelTranslate = glm::translate(modelTranslate, glm::vec3(-0.05f, 0.0f, 0.0f));
				//EarthTranslate = glm::translate(EarthTranslate, glm::vec3(-0.05f, 0.0f, 0.0f)); //(X, Y, Z)
				break;
			case SDLK_d:
				MoveRight = true;
				//modelTranslate = glm::translate(modelTranslate, glm::vec3(0.05f, 0.0f, 0.0f));
				//EarthTranslate = glm::translate(EarthTranslate, glm::vec3(0.05f, 0.f, 0.0f)); //(X, Y, Z)
				break;
			case SDLK_1:
				projectionMatrix = glm::ortho(-3.0f, 4.0f, -4.0f, 2.0f, -60.0f, 100.0f);
				break;
			case SDLK_0:
				projectionMatrix = glm::perspective(glm::radians(45.0f), (float)w / (float)h, 0.1f, 100.0f);
				break;
			case SDLK_2:
				//viewMatrix = glm::lookAt(glm::vec3(cam.camXPos, cam.camYPos, cam.camZPos));
				break;
			case SDLK_SPACE:
				SpawnBullet = true;
				break;
			}
		}

		if (event.type == SDL_KEYUP)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_a:
				MoveLeft = false;
				break;
			case SDLK_d:
				MoveRight = false;
				break;
			default:
				break;
			}
		}
	}
}
#endif
#endif
#endif
#endif


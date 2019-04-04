#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


using namespace std;

class Cubemap
{
private:
	GLuint skyboxVAO;
	GLuint skyboxVBO;

	glm::mat4 view;

	Shader shader;
	Camera *camera;
	Texture cubemapTexture;

	void CalculateDimension()
	{
		float skyboxVertices[] = {
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};

		glGenVertexArrays(1, &skyboxVAO);
		glGenBuffers(1, &skyboxVBO);
		glBindVertexArray(skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		// Cubemap Textures
		vector<const GLchar*> faces;
		faces.push_back("right");	faces.push_back("left");
		faces.push_back("top");		faces.push_back("bottom");
		faces.push_back("back");	faces.push_back("front");

		//Load the texture -- changing the middle value will give different textures -- "space"/"storm"/"lake"
		cubemapTexture = AssetController::LoadCubeMapTexture(faces, "storm", "cubemap");
	}

public:
	Cubemap(Shader &shader) { this->shader = shader; CalculateDimesions(); }

	void Draw(glm::mat4 viewMatrix)
	{
		glDepthFunc(GL_LEQUAL);
		this->shader.Use();
		view = glm::mat4(glm::mat3(viewMatrix));
		this->shader.SetMatrix4("view", view);

		glActiveTexture(GL_TEXTURE0);
		cubemapTexture.BindCubeMap();

		glBindVertexArray(skyboxVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6 * 2 * 3);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
	}
};
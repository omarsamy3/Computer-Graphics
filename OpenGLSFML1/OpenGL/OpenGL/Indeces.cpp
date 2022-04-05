
#include <iostream>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include<vector>
#include<gl/glm/glm.hpp>
#include<gl/glm/gtc/matrix_transform.hpp>
#include<gl/glm/gtc/type_ptr.hpp>
#include "Indeces.h"

using namespace std;

//Using the prototype of InitShader function from shaderFunction.cpp file.
GLuint InitShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name); //Gluint stands for (unsigned int)
//A prototype for the function SetUniformMatrix
void SetUniformMarix(glm::mat4& _model, GLuint _shaderId, const GLchar* name);
//A prototype for the function PrepareBuffers
void PrepareBuffers(float  vertices[28], int  indices[6], float sizeVertices, float sizeIndeces);

GLuint shaderId;
GLuint VBO, VAO, EBO;
float offsetX, offsetY;

const GLint WIDTH = 600, HEIGHT = 600;

int Init()
{
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		cout << "Error";
		getchar();
		return 1;
	}
	else
	{
		if (GLEW_VERSION_3_0)
			cout << "Driver support OpenGL 3.0\nDetails:\n";
	}
	cout << "\tUsing glew " << glewGetString(GLEW_VERSION) << endl;
	cout << "\tVendor: " << glGetString(GL_VENDOR) << endl;
	cout << "\tRenderer: " << glGetString(GL_RENDERER) << endl;
	cout << "\tVersion: " << glGetString(GL_VERSION) << endl;
	cout << "\tGLSL:" << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	glClearColor(1, 0, 0, 1);

	return 0;
}

void Update()
{
}

void Render()
{
	//Clear the layers on the screen before rendering.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#pragma region Offset_With_Arrows
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		if (offsetX < 0.9f) offsetX += 0.01f;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		if (offsetX > -0.9f) offsetX -= 0.01f;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		if (offsetY < 0.9f) offsetY += 0.01f;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		if (offsetY > -0.9f) offsetY -= 0.01f;
	}
#pragma endregion

#pragma region Movement_Color_Rotation_Scale
	//make a movement with arrows
	int uniformId = glGetUniformLocation(shaderId, "offset");
	glUniform3f(uniformId, offsetX, offsetY, 0.0f);

	//Change the color.
	int uniformId_Color = glGetUniformLocation(shaderId, "coloru");
	glUniform4f(uniformId_Color, offsetX + 0.2f, offsetY + 0.2f, 5.0f, 0.0f);

	//Actions on the model.
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(offsetX, offsetY, 0));
	model = glm::rotate(model, 90.0f, glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	SetUniformMarix(model, shaderId, "model");

	//View Matrix
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	SetUniformMarix(view, shaderId, "view");

	//Space Matrix
	glm::mat4 projection = glm::mat4::tmat4x4(1.0f);
	projection = glm::perspective(45.0f, (float)WIDTH / (float)HEIGHT, 0.01f, 100.0f);
	SetUniformMarix(projection, shaderId, "projection");


	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
#pragma endregion

	glUseProgram(shaderId);
}

void SetUniformMarix(glm::mat4& _model, GLuint _shaderId, const GLchar *name)
{
	int uniformId = glGetUniformLocation(_shaderId, name);
	glUniformMatrix4fv(uniformId, 1, GL_FALSE, glm::value_ptr(_model));
}

int main()
{
	sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "SFML works!", sf::Style::Close);

	if (Init()) return 1;

	//create an array of vertices.
	float vertices[] = {
		-0.5f, -0.5f, 0.0f  ,    0.2, 0, 1, 0,	//1st point
		0.5f, -0.5f,  0.0f  ,    0.2, 1, 1, 0,	//2nd point
		0.5f,  0.5f,  0.0f  ,    0.8, 0, 0, 0,  //3rd point
		-0.5,  0.5,   0.0f  ,    0.1, 1, 1, 0,	//4th point
	};
	int indices[] = {
		0, 1, 2,		//1st triangle
		0, 2, 3			//2nd triangle
	};

#pragma region PipeLine

	shaderId = InitShader("vertexshader.glsl", "fragmentshader.glsl");

	PrepareBuffers(vertices, indices, sizeof(vertices), sizeof(indices));
#pragma endregion

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{

			case sf::Event::Closed:
				window.close();
				break;
			}
		}
		Update();
		Render();

		window.display();
	}
	return 0;
}

void PrepareBuffers(float  vertices[28], int  indices[6], float sizeVertices, float sizeIndeces)
{
	glGenVertexArrays(1, &VAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeVertices, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndeces, indices, GL_STATIC_DRAW);

	//Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); //Binding
	glEnableVertexAttribArray(0);
	//Color
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float))); //Binding
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); //Unbinding.
}

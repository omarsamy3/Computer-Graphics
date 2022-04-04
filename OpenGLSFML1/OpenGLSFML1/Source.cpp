
#include <iostream>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include<vector>

using namespace std;

//Using the prototype of InitShader function.
GLuint InitShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name); //Gluint stands for (unsigned int)

GLuint shaderId; //Create a variable of the same type of the return of the function 'InitShader'
GLuint VBO, VAO; //VBO >> Vertix Buffer Object, VAO >> Vertix Array Object.
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
	//Clear the data on the screens before rendering.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		if(offsetX <= 0.5)
		offsetX += 0.001f;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		if (offsetX >= -0.5)
		offsetX -= 0.001f;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		if (offsetY <= 0.5)
		offsetY += 0.001f;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		if (offsetY >= -0.5)
		offsetY -= 0.001f;
	}

	//Use the shader (shaderId).
	glUseProgram(shaderId);

	int uniformId_Pos = glGetUniformLocation(shaderId, "offset");
	glUniform3f(uniformId_Pos, offsetX, offsetY, 0.0f);

	int uniformId_Color = glGetUniformLocation(shaderId, "coloru");
	glUniform4f(uniformId_Color, offsetX, offsetY, 0.5f, 1);

	//To define which vertex array to be drawn.
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);  //To start drawing.
}

int main()
{
	sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "SFML works!", sf::Style::Close);
	if (Init()) return 1;

	//Create an array of vertices.
	float vertices[] = {
	   -0.5f, -0.5f, 0.0f,    0.2, 0, 1,1,	    //1st point
		0.5f, -0.5f, 0.0f,    0.2, 1, 1,1,	    //2nd point
		0.5f, 0.5f, 0.0f,     0.8, 0, 0,1,		//3rd point

	   -0.5f, -0.5f, 0.0f,    1, 0, 0,1,		//1st point
		0.5f, 0.5f, 0.0f,     0, 1, 0,1,		//3rd point
	   -0.5f, 0.5f, 0.0f,     0.1, 1, 1, 1		//4th point
	};

#pragma region Prepare_Before_Rendering
	//Return a shaderId to use it again.
	shaderId = InitShader("vertexshader.glsl", "fragmentshader.glsl");
	//Generate only one vertex array and assign it to VAO variable.
	glGenVertexArrays(1, &VAO);
	//Generate only one buffer and assign to VBO.
	glGenBuffers(1, &VBO);

	//All codes and methods will be related to VAO, bind all vertix array and call VAO to call them.
	glBindVertexArray(VAO);
	//Bind for the ordinary buffer VBO with kind GL_ARRAY_BUFFER.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Get the position data and send the values to vertex shader, layout 0 which contains the position.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); //<<---Binding
	//To run the data comming from the previous function.
	glEnableVertexAttribArray(0);

	//Get the color data and send the values to vertex shader, layout 1 which contains the color. //<<---Binding
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	//To run the data comming from the previous function.
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);  //<<---UnBinding
	glBindVertexArray(0);
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
#include<gl\glew\glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include<fstream>
#include<gl\glm\glm.hpp>
#include<gl\glm\gtc\type_ptr.hpp>
#include<gl\glm\gtx\transform.hpp>
#include<vector>


using namespace std;
using namespace glm;

struct vertex
{
	vec3 position;
	vec2 uv;

	vertex() :uv(), position() {}
	vertex(vec3 pos, vec2 uv) :position(pos), uv(uv) {}
};

int programID;
GLuint VBO, IBO;
GLuint textureId;
GLuint InitShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name);

vertex quadVertices[] = {
		{ vec3(-0.5,0.5,0),vec2(0,1)},
		{ vec3(-0.5,-0.5,0),vec2(0,0)},
		{ vec3(0.5,-0.5,0),vec2(1,0)},
		{vec3(0.5,0.5,0),vec2(1,1)},
};
int indcies[] = {
	0,1,2,
	0,2,3
};

void CreateQuad()
{
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indcies), indcies, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(vertex), (char*)sizeof(vec3));
	glEnableVertexAttribArray(1);
}

void BindQuad()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(vertex), (char*)sizeof(vec3));
	glEnableVertexAttribArray(1);
}

void CompileShader()
{
	programID = InitShader("V_BasicShader.glsl", "F_BasicShader.glsl");
	glUseProgram(programID);
}

void LoadTexture()
{
	sf::Image image;
	if (!image.loadFromFile("AnimationFrames.png"))
	{
		cout << "failed loading image";
	}

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

int init()
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

	CompileShader();
	CreateQuad();
	LoadTexture();

	glClearColor(0, 0.5, 0.5, 1);

	return 0;
}

int currframe_x;
int currframe_y;

sf::Clock clck;

void update()
{
	if (clck.getElapsedTime().asMilliseconds() > 100)
	{
		clck.restart();

		currframe_x++;
		if (currframe_x == 5)
		{
			currframe_y++;
			currframe_y %= 2;

			int currentYLoc = glGetUniformLocation(programID, "current_y");
			glUniform1i(currentYLoc, currframe_y);
		}
		currframe_x %= 5;

		int currentYLoc = glGetUniformLocation(programID, "current_x");
		glUniform1i(currentYLoc, currframe_x);
	}
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

}

int main()
{
	sf::ContextSettings context;
	context.depthBits = 24;
	sf::Window window(sf::VideoMode(600, 600), "SFML works!", sf::Style::Close, context);

	if (init()) return 1;


	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

		}
		update();
		render();

		window.display();
	}
	return 0;
}
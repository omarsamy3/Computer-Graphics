
#include <iostream>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include<gl\glm\glm.hpp>
#include<gl\glm\gtc\type_ptr.hpp>
#include<gl\glm\gtx\transform.hpp>
#include<vector>

using namespace std;
using namespace glm;

struct vertex
{
	vec3 position;
	vec3 normal;
	vertex() {}
	vertex(vec3 _position, vec3 _normal) :position{ _position }, normal{ _normal }
	{
	}
	vertex(vec3 _position) :position{ _position }, normal(0, 0, 0)
	{
	}
};

const GLint WIDTH = 600, HEIGHT = 600;
GLuint Sphere_VBO,Cube_VBO, baseShaderProgramId, flatShadingProgramId, phongProgramId;
GLuint modelMatLoc, viewMatLoc, projMatLoc;
GLuint InitShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name);
mat4 ModelMat;

#pragma Region Sphere
vector<vertex> sphere_vertices;


vec3 Sphere_Core_vertices[4] = {
	vec3(0.0, 0.0, 1.0),
	vec3(0.0, 0.942809, -0.333333),
	vec3(-0.816497, -0.471405, -0.333333),
	vec3(0.816497, -0.471405, -0.333333)
};

void Triangle(vec3 a, vec3 b, vec3 c)
{
	vec3 normal = (a + b + c) / 3.0f;
	sphere_vertices.push_back(vertex(a, a));
	sphere_vertices.push_back(vertex(b, b));
	sphere_vertices.push_back(vertex(c, c));
}
void dividTriangle(vec3 a, vec3 b, vec3 c, int itertions)
{
	if (itertions > 0)
	{
		vec3 v1 = normalize(a + b);
		vec3 v2 = normalize(a + c);
		vec3 v3 = normalize(b + c);

		dividTriangle(a, v1, v2, itertions - 1);
		dividTriangle(v1, b, v3, itertions - 1);
		dividTriangle(v1, v3, v2, itertions - 1);
		dividTriangle(v2, v3, c, itertions - 1);
	}
	else
	{
		Triangle(a, b, c);
	}
}
void CreateSphere(int iterations)
{
	sphere_vertices.clear();
	dividTriangle(Sphere_Core_vertices[0], Sphere_Core_vertices[1], Sphere_Core_vertices[2], iterations);
	dividTriangle(Sphere_Core_vertices[0], Sphere_Core_vertices[3], Sphere_Core_vertices[1], iterations);
	dividTriangle(Sphere_Core_vertices[0], Sphere_Core_vertices[2], Sphere_Core_vertices[3], iterations);
	dividTriangle(Sphere_Core_vertices[3], Sphere_Core_vertices[2], Sphere_Core_vertices[1], iterations);


	glGenBuffers(1, &Sphere_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Sphere_VBO);
	glBufferData(GL_ARRAY_BUFFER, sphere_vertices.size() * sizeof(vertex), sphere_vertices.data(), GL_STATIC_DRAW);

	/*
	x,y,z,r,g,b,
	x,y,z,r,g,b,
	x,y,z,r,g,b,
	*/

	/*
	x,y,z,
	x,y,z,
	x,y,z,
	r,g,b,
	r,g,b,
	r,g,b,
	*/

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), (char*)(sizeof(vec3)));
	glEnableVertexAttribArray(1);
}
void BindSphere()
{
	glBindBuffer(GL_ARRAY_BUFFER, Sphere_VBO);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}
#pragma endregion 

void CompileShader()
{
	baseShaderProgramId = InitShader("V_BasicShader.glsl", "F_BasicShader.glsl");
	flatShadingProgramId = InitShader("V_FlatShadingShader.glsl", "F_FlatShadingShader.glsl");
	phongProgramId = InitShader("V_phong.glsl", "F_phong.glsl");
	glUseProgram(phongProgramId);
}

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

	CompileShader();
	CreateSphere(4);

	modelMatLoc = glGetUniformLocation(baseShaderProgramId, "modelMat");
	viewMatLoc = glGetUniformLocation(baseShaderProgramId, "viewMat");
	projMatLoc = glGetUniformLocation(baseShaderProgramId, "projMat");

	mat4 viewMat = lookAt(vec3(0, 0, 3), vec3(0, 0, 0), vec3(0, 1, 0));
	glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, value_ptr(viewMat));

	mat4 projMat = perspectiveFov(60.0f, 5.0f, 5.0f, 1.0f, 10.0f);
	glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));

	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);

	return 0;
}

float theta = 0;
void Update()
{
	theta += 0.001f;
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glUseProgram(baseShaderProgramId);
	//BindSphere();

	ModelMat = translate(glm::vec3(0, 0, 0)) *
		rotate(theta * 180 / 3.14f, vec3(1, 1, 1)) *
		scale(vec3(1, 1, 1));
	glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, value_ptr(ModelMat));

	glDrawArrays(GL_TRIANGLES, 0, sphere_vertices.size());
}

int main()
{
	sf::ContextSettings context;
	context.depthBits = 24;
	sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "SFML works!", sf::Style::Close, context);

	if (Init()) return 1;

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
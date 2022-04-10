
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

//This struct to include the used vertices
struct vertex
{
	vec3 position; 
	vec3 normal;   
	//Different 3 types of constructor.
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
vector<vertex> sphere_vertices; //Sphere_vertices is a vector of type vertex which is a struct has vec3 position and normal.

//These vertices are for drawing a simple sphere.     
vec3 Sphere_Core_vertices[4] = {
	vec3(0.0, 0.0, 1.0),
	vec3(0.0, 0.942809, -0.333333),
	vec3(-0.816497, -0.471405, -0.333333),
	vec3(0.816497, -0.471405, -0.333333)
};

void Triangle(vec3 a, vec3 b, vec3 c)
{
	vec3 normal = (a + b + c) / 3.0f;
	sphere_vertices.push_back(vertex(a, normal)); 
	sphere_vertices.push_back(vertex(b, normal)); 
	sphere_vertices.push_back(vertex(c, normal)); 
}
//This function is to divide the triangle with the number of (iteration - 1).
void dividTriangle(vec3 a, vec3 b, vec3 c, int iterations)
{
	if (iterations > 0)
	{
		vec3 v1 = normalize(a + b);
		vec3 v2 = normalize(a + c);
		vec3 v3 = normalize(b + c);

		dividTriangle(a , v1, v2, iterations - 1);
		dividTriangle(v1, b , v3, iterations - 1);
		dividTriangle(v1, v3, v2, iterations - 1);
		dividTriangle(v2, v3, c , iterations - 1);
	}
	else
	{
		Triangle(a, b, c); 
	}
}
void CreateSphere(int iterations)
{
	sphere_vertices.clear();//This is to clear the vertices if the function is called in the main wrongly.
	//Divide the sphere which has four faces by calling the dividTriangle. This is in CPU.
	dividTriangle(Sphere_Core_vertices[0], Sphere_Core_vertices[1], Sphere_Core_vertices[2], iterations); //Divide the Face 1 of the sphere.
	dividTriangle(Sphere_Core_vertices[0], Sphere_Core_vertices[3], Sphere_Core_vertices[1], iterations); //Divide the Face 2 of the sphere.
	dividTriangle(Sphere_Core_vertices[0], Sphere_Core_vertices[2], Sphere_Core_vertices[3], iterations); //Divide the Face 3 of the sphere.
	dividTriangle(Sphere_Core_vertices[3], Sphere_Core_vertices[2], Sphere_Core_vertices[1], iterations); //Divide the Face 4 of the sphere.

	//Book a buffer like a room, determine the owner and assign the data. This is to deal with GPU.
	glGenBuffers(1, &Sphere_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Sphere_VBO);
	glBufferData(GL_ARRAY_BUFFER, sphere_vertices.size() * sizeof(vertex), sphere_vertices.data(), GL_STATIC_DRAW); 
	//Static that I'll not change the data, but the dynamic will be changed in the run time.

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

	//How it will deal with data.
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), (char*)(sizeof(vec3)));
	glEnableVertexAttribArray(1);
}
void BindSphere()
{
	glBindBuffer(GL_ARRAY_BUFFER, Sphere_VBO);//Sphere_VBO is a buffer in CPU has the sphere data.
	
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}
#pragma endregion 

void CompileShader() //Compile shaders and will be called in init function.
{
	baseShaderProgramId = InitShader("V_BasicShader.glsl", "F_BasicShader.glsl");
	flatShadingProgramId = InitShader("V_FlatShadingShader.glsl", "F_FlatShadingShader.glsl");
	phongProgramId = InitShader("V_phong.glsl", "F_phong.glsl");

	glUseProgram(phongProgramId); // choose which shader program will be compiled
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
	cout << "\t Using glew " << glewGetString(GLEW_VERSION) << endl;
	cout << "\t Vendor: " << glGetString(GL_VENDOR) << endl;
	cout << "\t Renderer: " << glGetString(GL_RENDERER) << endl;
	cout << "\t Version: " << glGetString(GL_VERSION) << endl;
	cout << "\t GLSL:" << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	CompileShader(); //Pull the shader files data to compile.
	CreateSphere(5); //Create the sphere with the number of iteration you need.

	//This way is to get the location of the uniform vectors unlike the modern way using layout.
	modelMatLoc = glGetUniformLocation(baseShaderProgramId, "modelMat");
	viewMatLoc = glGetUniformLocation(baseShaderProgramId, "viewMat");
	projMatLoc = glGetUniformLocation(baseShaderProgramId, "projMat");

	//LookAt function is to set the position and the direction of the camera.
	mat4 viewMat = lookAt(vec3(0, 0, 5), vec3(0, 0, 0), vec3(0, 1, 0));
	glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, value_ptr(viewMat));

	mat4 projMat = perspectiveFov(60.0f, 5.0f, 5.0f, 1.0f, 10.0f);
	glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));

	glClearColor(0, 0, 1, 1);
	glEnable(GL_DEPTH_TEST); //To enable depth buffer to test the values of z-indices to keep them updated.

	return 0;
}

float theta = 0;
void Update()
{
	theta += 0.001f;
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //To clear the color buffer and the depth buffer before rendering.
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
	//Create a context Setting to update the window of sfml.
	sf::ContextSettings context;
	context.depthBits = 24; //give the depthbits a size of 24.
	sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "SFML works!", sf::Style::Close, context); //give the window the updated context.

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
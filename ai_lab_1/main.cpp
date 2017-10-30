#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include <string>

#include "Shader.h"
#include "Text.h"

bool keys[1024];
bool keyProcessed[1024];
const GLuint WIDTH = 800, HEIGHT = 600;

struct Point {
	GLfloat x;
	GLfloat y;
} graph[2000], graph1[2000], graph2[2000];

int power = 6;
float width = 10.0f;
float x = 0.5f;
float y = 1.0f;
float b = 500.0f;

GLuint VAO0, VBO0, VAO1, VBO1, VAO2, VBO2;

std::string getWord(float num);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void Do_Movement();
float getFunction(float x, float trapeziaWidth, float extremum, int trapeziaPower, int oldmax = 1, int newmax = 1);
float normalize(float x);
void getFunctionVertices(float trapeziaWidth, float extremum, int trapeziaPower, Point *graph);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "ai_lab_1", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glViewport(0, 0, WIDTH, HEIGHT);

	glfwSetKeyCallback(window, key_callback);

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(3);

	Shader graphShader("Shaders/graphShader.vert", "Shaders/graphShader.frag");
	Shader textShader("Shaders/vertexText.vert", "Shaders/fragmentText.frag");
	Text text;

	getFunctionVertices(width, 0.0f, power, graph);
	getFunctionVertices(width, 33.0f, power, graph1);
	getFunctionVertices(width, 66.0f, power, graph2);

	Point line[2];		//graph of time x = n
	line[0].x = 0.0f;	//Simply hardcode values
	line[0].y = 0.0f;
	line[1].x = 0.0f;
	line[1].y = 1.0f;

	Point point[1];
	point[0].x = 0.0f;
	point[0].y = 0.0f;

	GLuint VAO4, VBO4;
	glGenVertexArrays(1, &VAO4);
	glGenBuffers(1, &VBO4);
	glBindBuffer(GL_ARRAY_BUFFER, VBO4);
	glBindVertexArray(VAO4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(point), &point[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	GLuint VAO3, VBO3;
	glGenVertexArrays(1, &VAO3);
	glGenBuffers(1, &VBO3);
	glBindBuffer(GL_ARRAY_BUFFER, VBO3);
	glBindVertexArray(VAO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), &line[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	
	glGenVertexArrays(1, &VAO0);
	glGenBuffers(1, &VBO0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO0);
	glBindVertexArray(VAO0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(graph), &graph[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	glGenVertexArrays(1, &VAO1);
	glGenBuffers(1, &VBO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBindVertexArray(VAO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(graph1), &graph1[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBindVertexArray(VAO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(graph2), &graph2[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	bool drawFirstGraph = true;
	bool drawSecondGraph = true;
	bool drawThirdGraph = true;
	GLfloat y1, y2, y3;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		Do_Movement();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		std::stringstream ss, ss1, ss2;
		std::stringstream widthStream, powerStream;
		glm::mat4 model;
		glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);

		textShader.use();
		glUniformMatrix4fv(
			glGetUniformLocation(textShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		widthStream << width;
		text.RenderText(textShader, "Width: " + widthStream.str() , 500.0f, 570.0f, 0.4f, glm::vec3(1.0f));
		powerStream << power;
		text.RenderText(textShader, "Power: " + powerStream.str(), 500.0f, 550.0f, 0.4f, glm::vec3(1.0f));
		text.RenderText(textShader, "Use left right arrow to move vertical line, and up down arrows to change width, use A/D keys to change power.", 10.0f, 10.0f, 0.3f, glm::vec3(1.0f));
		if (drawFirstGraph)
			text.RenderText(textShader, "Big: ", 25.0f, 750.0f, 0.4f, glm::vec3(1.0f));
		if (drawSecondGraph)
			text.RenderText(textShader, "Middle: ", 25.0f, 725.0f, 0.4f, glm::vec3(1.0f));
		if (drawThirdGraph)
			text.RenderText(textShader, "Small: ", 25.0f, 705.0f, 0.4f, glm::vec3(1.0f));

	
		graphShader.use();
		glUniformMatrix4fv(
			glGetUniformLocation(graphShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::scale(model, glm::vec3(0.7, 0.7f, 0.7f));
		glUniformMatrix4fv(glGetUniformLocation(graphShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glUniform3f(glGetUniformLocation(graphShader.Program, "lineColor"), 1.0f, 0.0f, 0.0f);
		glBindVertexArray(VAO0);
		glDrawArrays(GL_LINE_STRIP, 0, 2000);

		glUniform3f(glGetUniformLocation(graphShader.Program, "lineColor"), 0.0f, 1.0f, 0.0f);
		glBindVertexArray(VAO1);
		glDrawArrays(GL_LINE_STRIP, 0, 2000);

		glUniform3f(glGetUniformLocation(graphShader.Program, "lineColor"), 0.0f, 0.0f, 1.0f);
		glBindVertexArray(VAO2);
		glDrawArrays(GL_LINE_STRIP, 0, 1300);

		glBindVertexArray(VAO3);
		model = glm::translate(model, glm::vec3(x, y, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(graphShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(glGetUniformLocation(graphShader.Program, "lineColor"), 0.5f, 0.5f, 0.5f);
		glDrawArrays(GL_LINES, 0, 2);

		glPointSize(10.0f);
		y1 = getFunction(x, width, 0.0f, power, 800, 100);
		y2 = getFunction(x, width, 33.0f, power, 800, 100);
		y3 = getFunction(x, width, 66.0f, power, 800, 100);

		glBindVertexArray(VAO4);
		graphShader.use();
		model = glm::translate(model, glm::vec3(x / 800.0f, y1 * 600.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(graphShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(glGetUniformLocation(graphShader.Program, "clr"), 1.0f, 1.0f, 1.0f);
		glDrawArrays(GL_POINTS, 0, 1);
		drawFirstGraph = false;
		std::string word = getWord(y1);
		ss << int(y1 * 100);
		text.RenderText(textShader, word + " Small: " + ss.str() + "%", 25.0f, 570.0f, 0.4f, glm::vec3(1.0f));

		glBindVertexArray(VAO4);
		graphShader.use();
		model = glm::translate(model, glm::vec3(x / 800.0f, y2 * 600.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(graphShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(glGetUniformLocation(graphShader.Program, "clr"), 1.0f, 1.0f, 1.0f);
		glDrawArrays(GL_POINTS, 0, 1);
		drawSecondGraph = false;
		word = getWord(y2);
		ss1 << int(y2 * 100);
		text.RenderText(textShader, word + " Middle: " + ss1.str() + "%", 25.0f, 550.0f, 0.4f, glm::vec3(1.0f));

		glBindVertexArray(VAO4);
		graphShader.use();
		model = glm::translate(model, glm::vec3(x / 800.0f, y3 * 600.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(graphShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(glGetUniformLocation(graphShader.Program, "clr"), 1.0f, 0.0f, 1.0f);
		glDrawArrays(GL_POINTS, 0, 1);
		drawThirdGraph = false;
		word = getWord(y3);
		ss2 << int(y3 * 100);
		text.RenderText(textShader, word + " Big: " + ss2.str() + "%", 25.0f, 530.0f, 0.4f, glm::vec3(1.0f));
	
		glLineWidth(4);
		glBindVertexArray(VAO3);
		graphShader.use();
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, 0, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 1.0f));
		glUniformMatrix4fv(glGetUniformLocation(graphShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(glGetUniformLocation(graphShader.Program, "lineColor"), 1.0f, 1.0f, 1.0f);
		glDrawArrays(GL_LINES, 0, 2);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(720, -65, 0.0f));
		model = glm::rotate(model, 90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(glGetUniformLocation(graphShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(glGetUniformLocation(graphShader.Program, "lineColor"), 1.0f, 1.0f, 1.0f);
		glDrawArrays(GL_LINES, 0, 2);
		glLineWidth(3);
		
		glBindVertexArray(VAO4);
		graphShader.use();
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-30, 405, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(graphShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(glGetUniformLocation(graphShader.Program, "model"), 1.0f, 1.0f, 1.0f);
		glDrawArrays(GL_POINTS, 0, 1);

		graphShader.use();
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(570, -15, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(graphShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(glGetUniformLocation(graphShader.Program, "model"), 1.0f, 1.0f, 1.0f);
		glDrawArrays(GL_POINTS, 0, 1);

		glPointSize(1.0f);
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}


std::string getWord(float num)
{
	if (num <= 1 && num >= 0.8)
		return "Absolutely";
	if (num < 0.8 && num >= 0.6)
		return "Relatively";
	if (num < 0.6 && num >= 0.4)
		return "Not really";
	if (num < 0.4 && num >= 0.2)
		return "Not";
	else
		return "Absolutely not";
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
			keyProcessed[key] = false;
		}
	}
}


void Do_Movement()
{
	if (keys[GLFW_KEY_RIGHT] && x <= 520)
		x += 1.0f;

	if (keys[GLFW_KEY_LEFT] && x >= 0)
		x -= 1.0f;

	if ((keys[GLFW_KEY_D] && !keyProcessed[GLFW_KEY_D]) && power <= 8)
	{
		power += 2;
		getFunctionVertices(10.0f, 0.0f, power, graph);
		glBindBuffer(GL_ARRAY_BUFFER, VBO0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(graph), graph, GL_STATIC_DRAW);

		getFunctionVertices(10.0f, 33.0f, power, graph1);
		glBindBuffer(GL_ARRAY_BUFFER, VBO1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(graph1), graph1, GL_STATIC_DRAW);

		getFunctionVertices(10.0f, 66.0f, power, graph2);
		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(graph2), graph2, GL_STATIC_DRAW);
		
		keyProcessed[GLFW_KEY_D] = true;
	}
		
	if ((keys[GLFW_KEY_A] && !keyProcessed[GLFW_KEY_A]) && power >= 6)
	{
		power -= 2;
		getFunctionVertices(width, 0.0f, power, graph);
		glBindBuffer(GL_ARRAY_BUFFER, VBO0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(graph), graph, GL_STATIC_DRAW);

		getFunctionVertices(width, 33.0f, power, graph1);
		glBindBuffer(GL_ARRAY_BUFFER, VBO1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(graph1), graph1, GL_STATIC_DRAW);

		getFunctionVertices(width, 66.0f, power, graph2);
		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(graph2), graph2, GL_STATIC_DRAW);

		keyProcessed[GLFW_KEY_A] = true;
	}

	if ((keys[GLFW_KEY_UP] && !keyProcessed[GLFW_KEY_UP]) && width <= 15.0f)
	{
		width += 0.5;
		getFunctionVertices(width, 0.0f, power, graph);
		glBindBuffer(GL_ARRAY_BUFFER, VBO0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(graph), graph, GL_STATIC_DRAW);

		getFunctionVertices(width, 33.0f, power, graph1);
		glBindBuffer(GL_ARRAY_BUFFER, VBO1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(graph1), graph1, GL_STATIC_DRAW);

		getFunctionVertices(width, 66.0f, power, graph2);
		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(graph2), graph2, GL_STATIC_DRAW);

		keyProcessed[GLFW_KEY_UP] = true;
	}

	if ((keys[GLFW_KEY_DOWN] && !keyProcessed[GLFW_KEY_DOWN]) && width >= 5.0f)
	{
		width -= 0.5;
		getFunctionVertices(width, 0.0f, power, graph);
		glBindBuffer(GL_ARRAY_BUFFER, VBO0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(graph), graph, GL_STATIC_DRAW);

		getFunctionVertices(width, 33.0f, power, graph1);
		glBindBuffer(GL_ARRAY_BUFFER, VBO1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(graph1), graph1, GL_STATIC_DRAW);

		getFunctionVertices(width, 66.0f, power, graph2);
		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(graph2), graph2, GL_STATIC_DRAW);

		keyProcessed[GLFW_KEY_DOWN] = true;
	}
}

float getFunction(float x, float trapeziaWidth, float extremum, int trapeziaPower, int oldmax, int newmax)
{
	return 1.0f / (1.0f + std::pow(((((x / oldmax)*newmax) - extremum) / trapeziaWidth), trapeziaPower));
}


float normalize(float x)
{
	return x / 20.0f;
}

void getFunctionVertices(float trapeziaWidth, float extremum, int trapeziaPower, Point * graph)
{
	for (int i = 0; i < 2000; i++)
	{
		graph[i].x = normalize(i);
		graph[i].y = getFunction(graph[i].x, trapeziaWidth, extremum, trapeziaPower, 1, 1);
	}
}
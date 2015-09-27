#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef _DEBUG
#	pragma comment(lib, "GL/glew32.lib")
#	pragma comment(lib, "OpenGL32.lib")
#	pragma comment(lib, "GLFW/GLFW_d.lib")
#else
#	pragma comment(lib, "GL/glew32.lib")
#	pragma comment(lib, "OpenGL32.lib")
#	pragma comment(lib, "GLFW/GLFW.lib")
#endif

const std::string	title	= u8"マンデルブロ";
constexpr int		width	= 640;
constexpr int		height	= 480;

double	centerX;
double	centerY;
float	scale = 2.f;

//	mouse wheel callback
void onMouseScroll(GLFWwindow* window, double h, double v) {
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	x = (x-320)/100;
	y = (y-240)/100;

	if(v > 0) {
		centerX	-= x;
		centerY	+= y;
		centerX	*= 1.1f;
		centerY	*= 1.1f;
		scale	*= 1.1f;
		centerX	+= x;
		centerY	-= y;
	}
	else if(v < 0) {
		centerX	-= x;
		centerY	+= y;
		centerX	/= 1.1f;
		centerY	/= 1.1f;
		scale	/= 1.1f;
		centerX	+= x;
		centerY	-= y;
	}
}


//	initialize graphics
GLFWwindow* initialize()
{
	//	init glfw
	if(glfwInit() == GL_FALSE)
	{
		throw std::runtime_error{ "glfwInit" };
	}

	GLFWwindow* window;
	if(!(window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr)))
	{
		throw std::runtime_error{ "glfwCreateWindow" };
	}

	glfwMakeContextCurrent(window);
	glfwSetScrollCallback(window, onMouseScroll);

	//	init glew
	if(glewInit())
	{
		throw std::runtime_error{ "glewInit" };
	}

	return window;
}


//	draw
void draw(GLuint sizeId, GLuint scaleId)
{
	//	matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(centerX, centerY, 0.);
	glScalef(scale, scale, 1.f);

	//	draw
	const float vertices[][4] = {
		{ -3.2f, -2.4f, 0.f, 1.f },
		{ +3.2f, -2.4f, 0.f, 1.f },
		{ -3.2f, +2.4f, 0.f, 1.f },
		{ +3.2f, +2.4f, 0.f, 1.f },
	};

	glUniform2f(sizeId, 3.2f, 2.4f);
	glUniform1f(scaleId, std::sqrt(scale));

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(4, GL_FLOAT, 0, vertices);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(vertices)/sizeof(vertices[0]));

	glDisableClientState(GL_VERTEX_ARRAY);
}


//	compile shader
GLuint compileShader(const std::string& path, GLenum type)
{
	//	open
	std::ifstream file{ path };

	if(!file)
	{
		throw std::runtime_error{ path };
	}

	//	get size
	auto size = file.seekg(0, std::ios::end).tellg();
	file.seekg(0, std::ios::beg);

	//	read
	std::string source;
	source.resize(static_cast<std::size_t>(size));
	
	file.read(&source[0], size);

	//	compile shader
	GLuint shader = glCreateShader(type);

	const char* src = source.c_str();
	glShaderSource(shader, 1, &src, nullptr);

	glCompileShader(shader);

	GLint result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

	if(result == GL_FALSE)
	{
		GLint len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

		char log[1024];
		glGetShaderInfoLog(shader, 1024, &len, log);

		std::cout << log << std::endl;

		throw std::runtime_error{ "glCompileShader" };
	}

	return shader;
}


//	entry point
int main()
{
	GLFWwindow* window = nullptr;

	try
	{
		//	initialize graphics
		window = initialize();

		//	init gl
		glEnable(GL_CULL_FACE);

		//	compile shaders
		GLuint vs = compileShader("vs.glsl", GL_VERTEX_SHADER);
		GLuint fs = compileShader("fs.glsl", GL_FRAGMENT_SHADER);

		GLuint program = glCreateProgram();
		glAttachShader(program, vs);
		glAttachShader(program, fs);

		glLinkProgram(program);
		glUseProgram(program);
	
		GLuint sizeId	= glGetUniformLocation(program, "size");
		GLuint scaleId	= glGetUniformLocation(program, "scale");

		glMatrixMode(GL_PROJECTION);
		glOrtho(-3.2, 3.2, -2.4, 2.4, 0., 1.);

		double posX, posY;
		glfwGetCursorPos(window, &posX, &posY);

		//	main loop
		while(!glfwWindowShouldClose(window))
		{
			//	clear
			glClear(GL_COLOR_BUFFER_BIT);

			//	update
			double x = posX, y = posY;
			glfwGetCursorPos(window, &posX, &posY);

			if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
				centerX += (posX - x)/100;
				centerY -= (posY - y)/100;
			}

			//	draw
			draw(sizeId, scaleId);

			//	flip
			glFlush();
			glfwSwapBuffers(window);
			glfwPollEvents();

			//	sleep
			std::this_thread::sleep_for(std::chrono::microseconds{1000000 / 60});
		}
	}
	catch(const std::exception& e)
	{
		std::cout << typeid(e).name()	<< std::endl;
		std::cout << e.what()			<< std::endl;
	}

	//	terminate
	glfwDestroyWindow(window);
	glfwTerminate();
}

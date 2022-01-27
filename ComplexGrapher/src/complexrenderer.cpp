#pragma once

#include "../include/complexrenderer.hpp"
#include "../include/complexmath.hpp"
#include "math.h"
	
namespace cpx {
	window* active_window = nullptr;



	shader::shader() {
		_handle = 0;
	}

	shader::shader(const char* vertexPath, const char* fragmentPath) {
		// thanks to https://learnopengl.com/Getting-started/Shaders for code
		const char* vShaderCode = R"(
			#version 330 core

			layout (location = 0) in vec2 aPos;
			uniform int uPointSize = 4;
			uniform float uZoom = 1;
			uniform vec2 uPos = vec2(0, 0);
			
			void main() {
				gl_PointSize = uPointSize; 
				gl_Position = vec4(aPos * uZoom  + uPos, 1, 1);
			}
			)";
		const char* fShaderCode = R"(
			#version 460 core
			out vec4 FragColor;
			uniform vec3 uColor = vec3(1, 0, 0);

			void main() {
				FragColor = vec4(uColor, 1);
			}
			)";
		unsigned int vertex, fragment;
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		_handle = glCreateProgram();
		glAttachShader(_handle, vertex);
		glAttachShader(_handle, fragment);
		glLinkProgram(_handle);
		glDeleteShader(vertex);
		glDeleteShader(fragment);


	}

	void shader::set_vec3(GLint location, float x, float y, float z) const {
		glUniform3f(location, x, y, z);
	}

	void shader::use() const {
		glUseProgram(_handle);
	}

	int shader::get_handle() const {
		return _handle;
	}
	window::window() : m_glwindow(nullptr), _xsize(1000), _ysize(600), _VBO(0), _VAO(0), _buffer_bytes_filled(0), _name("MyWindow"), _delta_time(0), _time_last(0), _zoom(1), _ucolor_location(0), _upoint_location(0), _uzoom_location(0), _upos_location(0), _point_size(4), xpos(0), ypos(0), _draw_mode(0), _sqrt_zoom(1) {}
	window::window(const char* Name, int Xsize, int Ysize) : m_glwindow(nullptr), _name(Name), _xsize(Xsize), _ysize(Ysize), _VBO(0), _VAO(0), _buffer_bytes_filled(0), _delta_time(0), _time_last(0), _zoom(1), _ucolor_location(0), _upoint_location(0), _uzoom_location(0), _upos_location(0), _point_size(4), xpos(0), ypos(0), _draw_mode(0), _sqrt_zoom(1) {}
	window::~window() {
	}
	int window::run() {
		// Load GLFW and GLAD and create the window
		glfwInit();
		glfwWindowHint(GLFW_SAMPLES, 4);
		m_glwindow = glfwCreateWindow(_xsize, _ysize, _name, 0, 0);
		if (m_glwindow == nullptr) {
			throw new exception("GLFW window creation failed.");
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent(m_glwindow);
		glfwSwapInterval(2);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			throw new exception("GLAD initialization failed.");
			return -1;
		}
		// Set callbacks
		glfwSetFramebufferSizeCallback(m_glwindow, _size_callback);
		glfwSetScrollCallback(m_glwindow, _scroll_callback);
	
		// Load shader
		m_shader = shader("vertshader.glsl", "fragshader.glsl");
		m_shader.use();
		_ucolor_location = glGetUniformLocation(m_shader.get_handle(), "uColor");
		_upoint_location = glGetUniformLocation(m_shader.get_handle(), "uPointSize");
		_uzoom_location = glGetUniformLocation(m_shader.get_handle(), "uZoom");
		_upos_location = glGetUniformLocation(m_shader.get_handle(), "uPos");

		_draw_mode = GL_LINE_STRIP;

		// Prepare OpenGL
		glViewport(0, 0, _xsize, _ysize);
		glClearColor(1, 1, 1, 1);
		glGenVertexArrays(1, &_VAO);
		glGenBuffers(1, &_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, _VBO);
		glBindVertexArray(_VAO);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, (void*)0);
		glEnableVertexAttribArray(0);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_MULTISAMPLE);
		active_window = this;
		_time_last = glfwGetTime();
		return 0;
	}
	int window::get_vbo() const {
		return _VBO;
	}
	int window::get_vao() const {
		return _VAO;
	}
	int window::get_key_state(int key_code) const {
		return glfwGetKey(m_glwindow, key_code);
	}
	void window::update() {
		glUniform2f(_upos_location, (float)xpos, (float)ypos);
		glfwSwapBuffers(m_glwindow);
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT);
		auto temp = glfwGetTime();
		_delta_time = temp - _time_last;
		_time_last = temp;
		
	}
	bool window::should_close() const {
		return glfwWindowShouldClose(m_glwindow);
	}
	void window::draw_point(float x, float y, color _Color) {
		m_shader.set_vec3(_ucolor_location, _Color.r, _Color.g, _Color.b);
		float _Vertices[2] = {
			x - (float)xpos, y - (float)ypos
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2, _Vertices, GL_STATIC_DRAW);
		glDrawArrays(_draw_mode, 0, 1);
	}
	void window::draw_points(const float* points, size_t Size, color _Color) {
		m_shader.set_vec3(_ucolor_location, _Color.r, _Color.g, _Color.b);
		glBufferData(GL_ARRAY_BUFFER, (GLsizei)(sizeof(float) * Size), points, GL_STATIC_DRAW);
		glDrawArrays(_draw_mode, 0, (GLsizei)Size - 1 );
	};
	int window::x_size() const noexcept {
		return _xsize;
	}
	int window::y_size() const noexcept {
		return _ysize;
	}
	double window::time_passed() const noexcept {
		return _time_last;
	}
	void window::set_point_size(int NewSize) {
		_point_size = NewSize;
		glUniform1i(_upoint_location, NewSize);
	}
	int window::get_point_size() const noexcept {
		return _point_size;
	}
	double window::get_zoom() const noexcept {
		return _zoom;
	}
	void window::set_zoom(double NewZoom) {
		_zoom = NewZoom;
		_sqrt_zoom = ::sqrt(_zoom);
		glUniform1f(_uzoom_location, (float)_zoom);
	}
	double window::delta_time() {
		return _delta_time;
	}
	unsigned int window::get_draw_mode() const noexcept {
		return _draw_mode;
	}
	void window::set_draw_mode(unsigned int _mode) {
		_draw_mode = _mode;
	}
	double window::sqrt_zoom() const noexcept {

		return _sqrt_zoom;
	}
	void window::_size_callback(GLFWwindow* win, int width, int height) {
		active_window->_xsize = width;
		active_window->_ysize = height;
		glViewport(0, 0, width, height);
	}
	void window::_scroll_callback(GLFWwindow* win, double xoffset, double yoffset) {
		active_window->set_zoom(active_window->_zoom + yoffset * active_window->_zoom / 10);
	}


}




#pragma once
#include <thread>
#include "../glad/glad.h"
#include "../glfw/glfw3.h"

namespace cpx {
	template<class T>
	class function;

	struct complex;

	namespace detail {
		template<class RetType, class T>
		struct _Compute_s {
			inline static void _Compute(T _FunctionN, float* _ArrN, size_t _AccN, bool _Inf, bool _Multi);
			inline static const size_t max_threads = std::thread::hardware_concurrency();
		};

		template<class T>
		struct _Compute_s<double, T> {
			inline static void _Compute(T _FunctionN, float* _ArrN, size_t _AccN, bool _Inf, bool _Multi);
			inline static const size_t max_threads = std::thread::hardware_concurrency();
		};
	}

	struct color {
		constexpr color(float R, float G, float B) : r(R), g(G), b(B) {}
		float r;
		float g;
		float b;
		color operator*(const color other) const {
			auto temp = *this;
			temp.r = (r + other.r) / 2;
			temp.g = (g + other.g) / 2;
			temp.b = (b + other.b) / 2;
			return temp;
		}
	};

	namespace colors {
		static constexpr color red = color(1, 0, 0);
		static constexpr color blue = color(0, 0, 1);
		static constexpr color green = color(0, 1, 0);
		static constexpr color black = color(0, 0, 0);
		static constexpr color purple = color(0.5f, 0, 0.5f);
		static constexpr color yellow = color(1, 1, 0);
	};

	typedef khronos_usize_t size_t;

	class shader {

	public:
		shader();
		shader(const char* vertexPath, const char* fragmentPath);
		void set_vec3(GLint location, float x, float y, float z) const;
		void use() const;
		int get_handle() const;
	private:
		int _handle;
	};

	class window {
	public:
		window();
		window(const char* Name, int Xsize, int Ysize);
		~window();                                                           
		int run();
		int get_vbo() const;
		int get_vao() const;
		// Returns 1 if the key is down and 0 if it is not.
		int get_key_state(int key_code) const;
		void update();
		bool should_close() const;
		void draw_point(float x, float y, color _Color = color{ 1, 0, 0 });
		void draw_points(const float* points, size_t Size, color _Color = color{ 1, 0, 0 });
		template<class T>
		inline void graph_function(T Function, color _Color = color{ 1, 0, 0 }) {
			m_shader.set_vec3(_ucolor_location, _Color.r, _Color.g, _Color.b);
			float* _Buffer = new float[Function.accuracy()];
			detail::_Compute_s<decltype(Function.func(0.0)), decltype(Function.func)>::_Compute(Function.func, _Buffer, Function.accuracy(), Function.infinite(), Function.multithreaded());
			glBufferData(GL_ARRAY_BUFFER, (GLsizei)(Function.accuracy() * sizeof(float)), _Buffer, GL_STATIC_DRAW);
			glDrawArrays(_draw_mode, 0, (GLsizei)(Function.accuracy() / 2));
			delete[] _Buffer;
		}
		template<class T>
		inline void graph_function(T Function, float* Buffer, color _Color = color{ 1, 0, 0 }) {
			m_shader.set_vec3(_ucolor_location, _Color.r, _Color.g, _Color.b);
			detail::_Compute_s<decltype(Function.func(0.0)), decltype(Function.func)>::_Compute(Function.func, Buffer, Function.accuracy(), Function.infinite(), Function.multithreaded());
			glBufferData(GL_ARRAY_BUFFER, (GLsizei)(Function.accuracy() * sizeof(float)), Buffer, GL_STATIC_DRAW);
			glDrawArrays(_draw_mode, 0, (GLsizei)(Function.accuracy() / 2));
		}
		inline void graph_function(float* Buffer, int size, color _Color = color{ 1, 0, 0 }) {
			m_shader.set_vec3(_ucolor_location, _Color.r, _Color.g, _Color.b);
			glBufferData(GL_ARRAY_BUFFER, (GLsizei)(size * sizeof(float)), Buffer, GL_STATIC_DRAW);
			glDrawArrays(_draw_mode, 0, (GLsizei)(size / 2));
		}
		int x_size() const noexcept;
		int y_size() const noexcept;
		double time_passed() const noexcept;
		double xpos;
		double ypos;
		void set_point_size(int NewSize);
		int get_point_size() const noexcept;
		double get_zoom() const noexcept;
		void set_zoom(double NewZoom);
		double delta_time();
		unsigned int get_draw_mode() const noexcept;
		void set_draw_mode(unsigned int _mode);
		double sqrt_zoom() const noexcept;
	private:
		shader m_shader;
		GLFWwindow* m_glwindow;
		int _xsize;
		int _ysize;
		double _time_last;
		double _delta_time;
		double _sqrt_zoom;
		double _zoom;
		size_t _buffer_bytes_filled;
		GLint _ucolor_location;
		GLint _uzoom_location;
		GLint _upoint_location;
		GLint _upos_location;
		int _point_size;
		GLuint _VBO;
		GLuint _VAO;
		const char* _name;
		unsigned int _draw_mode;
		static void _size_callback(GLFWwindow*, int, int);
		static void _scroll_callback(GLFWwindow*, double, double);
	};

	class exception {
	public:
		exception() : _what() {}
		exception(const char* What) : _what(What) {}
		inline const char* what() {
			return _what;
		}
	private:
		const char* _what;
	};

	extern window* active_window;
}
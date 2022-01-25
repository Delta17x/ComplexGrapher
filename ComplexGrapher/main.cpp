#include "include/complexmath.hpp"
#include <iostream>
#include <chrono>
#include <type_traits>
#include <functional>

using namespace cpx;
float bar = 0;
float total_time = 0;
size_t frame_count = 0;
constexpr double e = constants::e;


complex circle(double t) {
	complex ret = (constants::e ^ complex(0, t));
	return ret;
}

complex rotate(double t) {
	complex ret = e ^ complex(bar);
	return ret;
}

complex jeff(double t) {
	// exp : 1
	// sin + sinh : 7
	// cos + cosh : 7
	// multiplication : 12
	// division : 1

	auto temp = bar * exp(complex(t));
	complex s = sin(temp) + cos(temp) + tan(temp);
	return s;
}

complex bob(double t) {
	complex it = complex(t);
	complex s = exp(it * cos(t * bar)) * cos(it);
	return s;
}

complex jim(double t) {
	complex it = complex(t);
	return exp(it) * (sin(t * bar) * cos(t * bar));
}

complex emily(double t) {
	complex it = complex(t);
	return exp(it) * (sin(sin(cos(bar)) * t * cos(t * cos(bar))));
}

complex amy(double t) {
	complex it = complex(t);
	return exp(it) * sin((t + cos(t)) * cos(bar));
}

complex john(double t) {
	complex it = complex(t);
	for (int i = 1; i < t; i++) {
		it += exp(-it * i);
	}
	return it;
}


double fourier(double x) {  
	return integral([](double t) {return sin(t); }, 0, x);
}


double parabola(double x) {
	return x * x;
}

std::ostream& print_complex(complex s) {
	std::cout << s.re << " + " << s.im << "i";
	return std::cout;
}

int main() {

	window MyWindow("Complex Grapher", 1000, 600);
	MyWindow.run();
	MyWindow.set_zoom(10);
	glClearColor(0, 0, 0, 1);
	glLineWidth(1);
	while (!MyWindow.should_close()) {
		MyWindow.graph_function(function(parabola, 100000, 1));

		MyWindow.xpos -= 0.02 * ((double)MyWindow.get_key_state('A') - (double)MyWindow.get_key_state('D'));
		MyWindow.ypos -= 0.02 * ((double)MyWindow.get_key_state('S') - (double)MyWindow.get_key_state('W'));
		if (MyWindow.get_key_state('M')) {
			MyWindow.set_draw_mode(GL_POINTS);
		}
		if (MyWindow.get_key_state('N')) {
			MyWindow.set_draw_mode(GL_LINE_STRIP);
		}
		MyWindow.update();
	}
}
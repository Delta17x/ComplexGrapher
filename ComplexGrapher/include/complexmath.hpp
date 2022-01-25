#pragma once
//
#include "complexrenderer.hpp"
#include <math.h> 
#include <iostream>
namespace cpx {

	struct constants {
		constexpr static double pi = 3.14159265358979323846;
		constexpr static double e = 2.71828182845904523536;
	};

	struct complex {
		union {
			double real;
			double re;
		};
		union {
			double imaginary;
			double im;
		};
		constexpr complex() : re(0), im(0) {}
		constexpr complex(double Re) : re(Re), im(0) {}
		constexpr complex(double Re, double Im) : re(Re), im(Im) {}
		constexpr complex(const complex& other) : re(other.re), im(other.im) {}
		constexpr complex& operator=(const complex& other) {
			re = other.re;
			im = other.im;
			return *this;
		} 
		constexpr complex operator+(complex b) noexcept {
			return complex(re + b.re, im + b.im);
		}
		constexpr complex operator+(double b) noexcept {
			return complex(re + b, im);
		}
		constexpr complex operator-(complex b) noexcept {
			return complex(re - b.re, im - b.im);
		}
		constexpr complex operator-(double b) noexcept {
			return complex(re - b, im);
		}
		constexpr complex operator*(complex b) noexcept {
			return complex(re * b.re - im * b.im, re * b.im + im * b.re);
		}
		constexpr complex operator*(double b) noexcept {
			return complex(re * b, im * b);
		}
		constexpr complex operator/(complex b) noexcept {
			return complex(re * b.re + im * b.im, im * b.re - re * b.im) / (b.re * b.re + b.im * b.im);
		}
		constexpr complex operator/(double b) noexcept {
			return complex(re / b, im / b);
		}
		constexpr complex operator-() noexcept {
			return complex(-re, -im);
		}
		constexpr complex& operator+=(complex other) {
			re += other.re;
			im += other.im;
			return *this;
		}
		constexpr complex& operator+=(double other) {
			re += other;
			return *this;
		}
		constexpr complex& operator-=(complex other) {
			re += other.re;
			im += other.im;
			return *this;
		}
		constexpr complex& operator-=(double other) {
			re -= other;
			return *this;
		}
		constexpr complex& operator*=(complex b) {
			*this = *this * b;
			return *this;
		}
		constexpr complex& operator*=(double other) {
			re *= other;
			im *= other;
			return *this;
		}
		constexpr complex& operator/=(complex other) {
			*this = *this / other;
			return *this;
		}
		constexpr complex& operator/=(double other) {
			re /= other;
			im /= other;
			return *this;
		}
	};
	constexpr complex operator-(double a, complex b) noexcept {
		return complex(-b.re + a, -b.im);
	}
	constexpr complex operator+(double a, complex b) noexcept {
		return complex(b.re + a, b.im);
	}
	constexpr complex operator*(double a, complex b) noexcept {
		return complex(b.re * a, b.im * a);
	}
	constexpr complex operator/(double a, complex b) noexcept {
		return a * complex(b.re, -b.im) / (b.re * b.re + b.im * b.im);
	}

	complex operator^(double base, complex a);
	complex operator^(complex a, double pow);
	complex operator^(complex a, complex pow);

	namespace detail {
		template<class RetType, class T>
		inline void _Compute_s<RetType, T>::_Compute(T _FunctionN, float* _ArrN, size_t _AccN, bool _Inf, bool _Multi) {
			double temp;
			if (_Inf)
				temp = 4 + 400.0 / active_window->get_zoom();
			else
				temp = 4 + 400.0 / active_window->sqrt_zoom();
			double _T = -temp;
			double _Inc = 4 * temp / _AccN;
			auto _StretchRemove = (active_window->x_size() / active_window->y_size());
			if (_Multi) {
				size_t step_size = (size_t)(_AccN / max_threads);
				std::thread* threads = new std::thread[max_threads];

				auto _F = [&](size_t th_num) { /* compute function for threads 0 - (max_threads - 1) */
					for (size_t i = th_num * step_size; i < (th_num + 1) * step_size; i += 2) {
						complex _Computed = _FunctionN(_T);
						_ArrN[i] = (float)_Computed.re;
						_ArrN[i + 1] = (float)_Computed.im * _StretchRemove;
						_T += _Inc;
					}	};

				for (size_t i = 0; i < max_threads - 1; i++) {
					threads[i] = std::thread(_F, i);
				}

				threads[max_threads - 1] = std::thread([&](size_t th_num) { /* final iteration, seperated from others so that we dont calculate more that _AccN */
					for (size_t i = step_size * (max_threads - 1) - 1; i < _AccN; i += 2) {
						complex _Computed = _FunctionN(_T);
						_ArrN[i] = (float)_Computed.re;
						_ArrN[i + 1] = (float)_Computed.im * _StretchRemove;
						_T += _Inc;
					}	}, max_threads - 1);

				for (size_t i = 0; i < max_threads; i++) {
					threads[i].join();
				}

				delete[] threads;
				return;
			}
			for (size_t i = 0; i < _AccN; i += 2) {
				complex _Computed = _FunctionN(_T);
				_ArrN[i] = (float)_Computed.re;
				_ArrN[i + 1] = (float)(_Computed.im * _StretchRemove);
				_T += _Inc;
			}	
		}

		template<class T>
		inline void _Compute_s<double, T>::_Compute(T _FunctionN, float* _ArrN, size_t _AccN, bool _Inf, bool _Multi) {
			double temp;
			if (_Inf)
				temp = 4 + 400.0 / active_window->get_zoom();
			else
				temp = 4 + 400.0 / active_window->sqrt_zoom();
			double _T = -temp;
			double _Inc = 4 * temp / _AccN;
			auto _StretchRemove = (active_window->x_size() / active_window->y_size());
			if (_Multi) {
				size_t step_size = (size_t)(_AccN / max_threads);
				std::thread* threads = new std::thread[max_threads];

				auto _F = [&](size_t th_num) { /* compute function for threads 0 - (max_threads - 1) */
					for (size_t i = th_num * step_size + th_num * step_size % 2 == 0 ? 1 : 0; i < (th_num + 1) * step_size; i += 2) {
						_ArrN[i] = (float)_T;
						_ArrN[i + 1] = (float)_FunctionN(_T) * _StretchRemove;
						_T += _Inc;
					}	};

				for (size_t i = 0; i < max_threads - 1; i++) {
					threads[i] = std::thread(_F, i);
				}

				threads[max_threads - 1] = std::thread([&]() { /* final iteration, seperated from others so that we dont calculate more that _AccN */
					for (size_t i = step_size * (max_threads - 1); i < _AccN - 1; i += 2) {
						_ArrN[i] = (float)_T;
						_ArrN[i + 1] = (float)_FunctionN(_T) * _StretchRemove;
						_T += _Inc;
					}	});

				for (size_t i = 0; i < max_threads; i++) {
					threads[i].join();
				}

				delete[] threads;
				return;
			}
			for (size_t i = 0; i < _AccN; i += 2) {
				_ArrN[i] = (float)_T;
				_ArrN[i + 1] = (float)_FunctionN(_T) * _StretchRemove;
				_T += _Inc;
			}
		}
	}

	template<class FuncType>
	class function {
	public:
		using type = FuncType;
		constexpr function() {
			func = nullptr;
			_accuracy = 0;
		};
		constexpr function(FuncType f_of_x, size_t Accuracy = 10000, bool MultiThreaded = true, bool IsInfinite = false) : func(f_of_x), _inf(IsInfinite), _accuracy(Accuracy), _multithreaded(MultiThreaded) {}
		FuncType func;
		inline bool infinite() const noexcept {
			return _inf;
		}
		inline complex operator()(double X) const {
			return func(X);
		};
		inline size_t accuracy() const noexcept {
			return _accuracy;
		}
		inline bool multithreaded() const noexcept {
			return _multithreaded;
		}
	private:
		bool _multithreaded;
		size_t _accuracy;
		bool _inf;
	};

	template<class FuncType>
	class integral {
	public:
		constexpr integral() : func(), lower(0), upper(0), accuracy(1), _computed() {}
		integral(FuncType f_of_x, double Lower = 0, double Upper = 1, size_t Accuracy = 1000, bool Auto_compute = true) : func(f_of_x), lower(Lower), upper(Upper), accuracy(Accuracy), _computed() {
			if (Auto_compute) {
				compute();
			}
		}
		void compute() {
			_computed = complex();
			double _delta = (upper - lower) / accuracy;
			for (int i = 1; i < accuracy; i++) {
				_computed += func(lower + _delta * i) * _delta;
			}
		}
		complex value() const {
			return _computed;
		}
		operator complex() const {
			return _computed;
		}
		operator double() const {
			return _computed.re;
		}
		FuncType func;
		double lower;
		double upper;
		size_t accuracy;
	private:
		complex _computed;
	};
	// Returns the natural log of the number, or the natural log of -number + PI * I if the number is negative.
	inline complex natural_log(double X) {
		if (X > 0) {
			return complex(::log(X), 0);
		}
		else { // X <= 0
			return complex(::log(-X), constants::pi);
		}
	}
	inline double mod(complex X) {
		return ::sqrt(X.re * X.re + X.im * X.im);
	}
	inline double arg(complex X) {
		return ::atan2(X.im, X.re);
	}
	inline complex log(complex X) {
		return complex(::log(X.re * X.re + X.im * X.im) / 2, arg(X));
	}
	inline complex sin(complex X) {
		return complex(::sin(X.re) * ::cosh(X.im), ::cos(X.re) * ::sinh(X.im));
	}
	inline complex cos(complex X) {
		return complex(::cos(X.re) * ::cosh(X.im), -::sin(X.re) * ::sinh(X.im));
	}
	inline complex tan(complex X) {
		auto _Double = 2 * X;
		return complex(::sin(_Double.re), ::sinh(_Double.im)) / (::cos(_Double.re) + ::cosh(_Double.im));
	}
	inline complex exp(complex X) {
		auto _BaseToRe = ::exp(X.real);
		return complex(_BaseToRe * ::cos(X.im), _BaseToRe * ::sin(X.im));
	}
	inline complex sqrt(complex X) {
		auto _R = mod(X);
		return ::sqrt(_R) * (X + _R) / mod(X + _R);
	}
	inline complex operator^(double base, complex co) {
		return exp(natural_log(base) * co);
	}
	inline complex operator^(complex a, double power) {
		complex _Prod = log(a) * power;
		// Exp function 
		auto _BaseToRe = ::exp(_Prod.re);
		return complex(_BaseToRe * ::cos(_Prod.im),
			_BaseToRe * ::sin(_Prod.im));
	}
	inline complex operator^(complex a, complex power) {
		complex _Prod = log(a) * power;
		// Exp function
		auto _BaseToRe = ::exp(_Prod.re);
		return complex(_BaseToRe * ::cos(_Prod.im),
			_BaseToRe * ::sin(_Prod.im));
	}
	inline complex pow_complex(complex base, double a) {
		return base ^ a;
	}
	inline complex pow_complex(double base, complex a) {
		return base ^ a;
	}
	inline complex pow_complex(complex base, complex a) {
		return base ^ a;
	}
}
#pragma once

#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <windows.h>
#include <stdexcept>
#include <iostream>
#include <optional>
#include <direct.h>
#include <stdint.h>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <numeric>
#include <cerrno>
#include <vector>
#include <math.h>
#include <string>
#include <omp.h>
#include <ctime>
#include <array>
#include <tuple>
#include <map>

#include <SDL.h>
#include <SDL_render.h>
#include <SFML/Audio.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

#undef min
#undef max
#undef clamp
#undef near
#undef far

#define PI          float(3.14159265f)
#define TWO_PI      float(6.28318531f)
#define INVERTED_PI float(0.31830989f)
#define DEG_RAD     float(0.01745329f)

#define W1 "+"
#define W2 "-"
#define W3 "|"
#define W4 "*"
#define W5 "g"
#define OO " "

enum Keys {
	NONE,
	KEY_W,
	KEY_A,
	KEY_S,
	KEY_D,
	KEY_L_ARROW,
	KEY_R_ARROW,
	KEY_SHIFT
};

inline uint32_t rgba(const uint8_t& red, const uint8_t& green, const uint8_t& blue, const uint8_t& alpha = 255) {
	return (alpha << 24) | (red << 16) | (green << 8) | blue;
}

inline Keys getKey(const SDL_Keycode& key) {
		switch (key) {
		case SDLK_w: return Keys::KEY_W;
		case SDLK_a: return Keys::KEY_A;
		case SDLK_s: return Keys::KEY_S;
		case SDLK_d: return Keys::KEY_D;
		case SDLK_LEFT :  return Keys::KEY_L_ARROW;
		case SDLK_RIGHT:  return Keys::KEY_R_ARROW;
		case SDLK_LSHIFT: return Keys::KEY_SHIFT;
		default: return Keys::NONE;
	};
}

inline float mapRange(float value, float a, float b, float c, float d) {
	value = std::min(std::max(value, a), b);
	float mappedValue = (value - a) / (b - a) * (d - c) + c;
	return mappedValue;
}

inline int mapRange(int value, int a, int b, int c, int d) {
	value = std::min(std::max(value, a), b);
	int mappedValue = static_cast<int>(static_cast<float>(value - a) / static_cast<float>(b - a) * static_cast<float>(d - c) + c);
	return mappedValue;
}

inline bool inRange(double value, double a, double b) {
	if (value > a && value <= b) return true;
	return false;
}
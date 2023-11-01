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
	KEY_SPACE,
	KEY_SHIFT
};

inline Keys getKey(const SDL_Keycode& key) {
		switch (key) {
		case SDLK_w: return Keys::KEY_W;
		case SDLK_a: return Keys::KEY_A;
		case SDLK_s: return Keys::KEY_S;
		case SDLK_d: return Keys::KEY_D;
		case SDLK_LEFT :  return Keys::KEY_L_ARROW;
		case SDLK_RIGHT:  return Keys::KEY_R_ARROW;
		case SDLK_SPACE:  return Keys::KEY_SPACE;
		case SDLK_LSHIFT: return Keys::KEY_SHIFT;
		default: return Keys::NONE;
	};
}

inline mat4 mulmat4(mat4 a, mat4 b) {
	mat4 Result = mat4();
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 4; ++k) {
				Result[i][j] += a[i][k] * b[k][j];
			}
		}
	}
	return Result;
}

inline vec3 triangleNormal(const vec3& v1, const vec3& v2, const vec3& v3) {
	return normalize(cross(v3 - v1, v2 - v1));
}

inline void rotateAroundAnchor(vec3& position, const vec3& anchor, vec3 angle) {
	mat4 translation = translate(mat4(1.0f), anchor);
	mat4 rotationX = rotate(mat4(1.0f), angle.x, vec3(1.0f, 0.0f, 0.0f));
	mat4 rotationY = rotate(mat4(1.0f), angle.y, vec3(0.0f, 1.0f, 0.0f));
	mat4 rotationZ = rotate(mat4(1.0f), angle.z, vec3(0.0f, 0.0f, 1.0f));
	mat4 rotation = rotationX * rotationY * rotationZ;
	mat4 invTranslation = translate(mat4(1.0f), -anchor);
	vec4 new_pos = vec4(position, 1.0f);
	new_pos = translation * rotation * invTranslation * new_pos;
	position = vec3(new_pos);
}

inline float distancePlainToPoint(const vec4& plane, const vec3& point) {
	float numerator = abs(plane.x * point.x + plane.y * point.y + plane.z * point.z + plane.w);
	float denominator = sqrt(plane.x * plane.x + plane.y * plane.y + plane.z * plane.z);
	return numerator / denominator;
}
#pragma once

#include "include.hpp"

struct Texture {
	vector<vector<vec3>> data;
	int width, height;

	Texture();
	Texture(const char* imagePath);

	void loadImage(const char* imagePath);
	vec3 getColor(const vec2& uv) const;
};
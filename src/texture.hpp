#pragma once

#include "include.hpp"

struct Texture {
	vector<vector<uvec4>> data;
	int width, height;

	Texture();
	Texture(const char* imagePath);

	void loadImage(const char* imagePath);
	uvec4 getColor(const double& u, const double& v) const;
};
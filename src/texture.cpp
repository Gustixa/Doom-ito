#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture() {
	width = 0;
	height = 0;
	data = vector<vector<vec3>>();
}

Texture::Texture(const char* imagePath) {
	loadImage(imagePath);
}

void Texture::loadImage(const char* imagePath) {
	int channels;
	unsigned char* image_data = stbi_load(imagePath, &width, &height, &channels, 3);

	if (!image_data) {
		cerr << "Error loading image." << endl;
		exit(1);
	}
	vector<vector<vec3>> result;

	for (int y = 0; y < height; ++y) {
		vector<vec3> row;
		for (int x = 0; x < width; ++x) {
			int offset = (y * width + x) * 3;

			vec3 pixel(
				image_data[offset] / 255.0f,
				image_data[offset + 1] / 255.0f,
				image_data[offset + 2] / 255.0f
			);

			row.push_back(pixel);
		}
		result.push_back(row);
	}
	data = result;
	stbi_image_free(image_data);
}

vec3 Texture::getColor(const vec2& uv) const {
	const size_t pixelX = size_t(uv.x * float(width)) % width;
	const size_t pixelY = size_t(float(height) - uv.y * float(height)) % height;
	return data[pixelY][pixelX];
}
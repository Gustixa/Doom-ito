#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hpp"

Texture::Texture() {
	width = 0;
	height = 0;
	data = vector<vector<uvec4>>();
}

Texture::Texture(const char* imagePath) {
	loadImage(imagePath);
}

void Texture::loadImage(const char* imagePath) {
	int channels;
	unsigned char* image_data = stbi_load(imagePath, &width, &height, &channels, 4);

	if (!image_data) {
		cerr << "Error loading image." << endl;
		exit(1);
	}
	vector<vector<uvec4>> result;

	for (int y = 0; y < height; ++y) {
		vector<uvec4> row;
		for (int x = 0; x < width; ++x) {
			int offset = (y * width + x) * 4;

			uvec4 pixel(
				image_data[offset],
				image_data[offset + 1],
				image_data[offset + 2],
				image_data[offset + 3]
			);

			row.push_back(pixel);
		}
		result.push_back(row);
	}
	data = result;
	stbi_image_free(image_data);
}

uvec4 Texture::getColor(const double& u, const double& v) const {
	if (u >= 0.0f && u <= 1.0f && v >= 0.0f && v <= 1.0f) {
		const size_t pixelX = size_t(u * float(width)) % width;
		const size_t pixelY = size_t(v * float(height)) % height;
		return data[pixelY][pixelX];
	}
	return uvec4(255, 0, 255, 255);
}
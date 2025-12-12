#include "Texture.h"
#include <cstdio>
#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image/stb_image_write.h>

static GLuint f_texture = GL_NONE;

void LoadImage(Image* image, int width, int height)
{
	image->pixels.resize(width * height);
	image->width = width;
	image->height = height;
	image->channels = 4;
}

void LoadImage(Image* image, const char* filename)
{
	stbi_uc* pixels = stbi_load(filename, &image->width, &image->height, &image->channels, 4);
	if (image->channels != 4)
		printf("INFO: Converted image %s from %i channels to 4 channels\n", filename, image->channels);
	image->channels = 4;

	image->pixels.resize(image->width * image->height);
	memcpy(image->pixels.data(), pixels, image->pixels.size() * sizeof(Pixel));
	stbi_image_free(pixels);
}

void UnloadImage(Image* image)
{
	image->pixels.resize(0);
	image->width = image->height = image->channels -1;
}

void LoadImageGradient(Image* image, Vector3 uv_00, Vector3 uv_10, Vector3 uv_01, Vector3 uv_11)
{
	for (int y = 0; y < image->height; y++)
	{
		for (int x = 0; x < image->width; x++)
		{
			Vector2 uv{ x / (float)image->width, y / (float)image->height };
			uv.y = 1.0f - uv.y;

			Vector3 a = Vector3Lerp(uv_00, uv_10, uv.x);
			Vector3 b = Vector3Lerp(uv_01, uv_11, uv.x);
			Vector3 c = Vector3Lerp(a, b, uv.y);

			Pixel& pixel = image->pixels[y * image->width + x];
			pixel.r = c.x * 255.0f;
			pixel.g = c.y * 255.0f;
			pixel.b = c.z * 255.0f;
		}
	}
}

void SaveImage(const char* filename, const Image& image)
{
	assert(image.channels == 4);
	stbi_write_png(filename, image.width, image.height, image.channels, image.pixels.data(), 0);
}

void LoadTexture(Texture* texture, const Image& image)
{
    GLuint handle = GL_NONE;
    glGenTextures(1, &handle);
	assert(handle != GL_NONE && !image.pixels.empty() && image.width > 0 && image.height > 0 && image.channels == 4);

    glBindTexture(GL_TEXTURE_2D, handle);

	// What happens when our uv's exceed 0 & 1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// What happens when texture is bigger or smaller than our texture (linear for burr, good for high-res but nearest is best for now)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// OpenGL's texture coordinates (S, T) differ from (U, V) such that Y is inverted, so flip our image vertically before uploading to the GPU
	stbi__vertical_flip((void*)image.pixels.data(), image.width, image.height, sizeof(Pixel));

	// Describe texture data and upload to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels.data());

	// Unbind current texture so we don't accidentally overwrite textures
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	// Make our internal texture representation reflect GPU texture
	texture->width = image.width;
	texture->height = image.height;
	texture->channels = image.channels;
	texture->handle = handle;
}

void UnloadTexture(Texture* texture)
{
    glDeleteTextures(1, &texture->handle);
    texture->handle = GL_NONE;
    texture->width = texture->height = -1;
}

void BeginTexture(const Texture& texture)
{
	assert(f_texture == GL_NONE && texture.handle != GL_NONE);
	glBindTexture(GL_TEXTURE_2D, texture.handle);
	f_texture = texture.handle;
}

void EndTexture()
{
	assert(f_texture != GL_NONE);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	f_texture = GL_NONE;
}

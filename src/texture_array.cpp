
#include "texture_array.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned char *TextureArray::loadTexture(std::string_view path, int &width, int &height, int &channels) {
    stbi_set_flip_vertically_on_load(true);

    return stbi_load(path.data(), &width, &height, &channels, 0);
}

void TextureArray::freeTexture(unsigned char *data) {
    stbi_image_free(data);
}

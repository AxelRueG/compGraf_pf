#ifndef LOAD_TEXTURE_H
#define LOAD_TEXTURE_H
#include <GL/gl.h>

struct Texture {
  GLuint id;
  int w,h;
  bool has_alpha;
  unsigned char *data;
};

Texture load_texture(const char *fname, bool generate_mipmaps=false);

void unload_texture(unsigned char *data);
  
#endif


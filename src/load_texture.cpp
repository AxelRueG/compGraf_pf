#include <GL/glu.h>
#include <GL/glext.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "load_texture.h"

Texture load_texture(const char *fname, bool generate_mipmaps) {
  int x=0,y=0,n=0;
  unsigned char *image = stbi_load(fname, &x, &y, &n, 0);
  if (!image) return {};
  GLuint id;
  glGenTextures(1,&id);
  glBindTexture(GL_TEXTURE_2D, id);
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  if (generate_mipmaps) {
    gluBuild2DMipmaps(GL_TEXTURE_2D, n, x, y,  n==3 ? GL_RGB:GL_RGBA, GL_UNSIGNED_BYTE, image);
    float aniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso); 
  } else 
    glTexImage2D(GL_TEXTURE_2D, 0, n, x, y, 0, n==3 ? GL_RGB:GL_RGBA, GL_UNSIGNED_BYTE, image);
  return {id,x,y,n==4,image};
}


void unload_texture(unsigned char * data) {
  stbi_image_free(data);
}


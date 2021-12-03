#include <iostream> // cout
#include <cmath> // sin cos
#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <GLUT/glut.h>
#else
# include <GL/gl.h>
# include <GL/glut.h>
#endif
#include "drawPartes.hpp"

static const float color0[] = {1,.8,.1};

// ctes y función auxiliar para definir propiedades del material y del modo de
// renderizado para cada pieza
enum MaterialType { 
  MT_None,      ///< solo color, sin textura ni alpha ni iluminación
  MT_Texture,   ///< con textura, pero sin iluminación
  MT_Glass,     ///< color con transparencia, sin iluminación ni textura
  MT_Lights_On, ///< este está solo para separa los que usan iluminación de los que no
  MT_Metal,     ///< con iluminación y sin textura, con ctes de material que simulan metal
  MT_Rubber,    ///< con iluminación y sin textura, con ctes de material que simulan goma/caucho
};

static void set_material(MaterialType type, const float color[]=color0) {
  static MaterialType prev_type = MT_None;
  if (prev_type!=type) {
    (type==MT_Glass   ?glEnable:glDisable)(GL_BLEND); 
    (type==MT_Texture ?glEnable:glDisable)(GL_TEXTURE_2D);
    (type>MT_Lights_On?glEnable:glDisable)(GL_LIGHTING);
    if (type==MT_Metal) {
      glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,89);
      float spec[] = {1.f,1.f,1.f};
      glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,spec);
    } else if (type==MT_Rubber) {
      glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,10);
      float spec[] = {.4f,.4f,.4f};
      glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,spec);
    }
  }
  glColor4f(color[0],color[1],color[2],type==MT_Glass?.75:1);
  prev_type = type;
};

void drawEjes() {
  // ejes
  glLineWidth(4); set_material(MT_None);
  glBegin(GL_LINES);
    glColor3f(1,0,0); glVertex3d(0,0,0); glVertex3d(1,0,0);
    glColor3f(0,1,0); glVertex3d(0,0,0); glVertex3d(0,1,0);
    glColor3f(0,0,1); glVertex3d(0,0,0); glVertex3d(0,0,1);
  glEnd();
  glLineWidth(2);
}

void drawCube() {
  static float aux_color_1[] = {1,1,1};
  set_material(MT_None,aux_color_1);

  // plano para apoyar el pez
  static float aux_color_2[] = {.5,.5,.5};
  set_material(MT_Glass,aux_color_2);
  glBegin(GL_QUADS);
    glVertex3f(-1,-1,0);
    glVertex3f(-1, 1,0);
    glVertex3f( 1, 1,0);
    glVertex3f( 1,-1,0);
  glEnd();
}

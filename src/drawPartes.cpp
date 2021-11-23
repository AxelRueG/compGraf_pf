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

static const float color0[] = {1,.8,.1}; // casco
static const float color1[] = {1,.8,.65}; // chasis 1
static const float color2[] = {1,.3,.1}; // chasis 2
static const float color3[] = {1,.3,.1}; // alerones

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

void drawRueda(int lod) {
  
  const double DOS_PI= 8*atan(1.0);
  static int llod=-1;
  static double *cosv=NULL;
  static double *sinv=NULL;
  static double dr;
  if (llod!=lod) {
    llod=lod;
    if (cosv) delete []cosv;
    if (sinv) delete []sinv;
    cosv=new double[lod+1];
    sinv=new double[lod+1];
    dr=DOS_PI/lod;
    double r=0;
    for (int i=0;i<=lod;i++) {
      cosv[i]=cos(r)/2;
      sinv[i]=sin(r)/2;
      r+=dr;
    }
  }
  
  static float aux_color_1[] = {.1,.1,.1};
  set_material(MT_Rubber,aux_color_1);
  glBegin(GL_QUADS);
    for (int i=0;i<lod;i++) {
      //capa de afuera
      glNormal3f(0,cosv[i],sinv[i]);
      glVertex3f(1,cosv[i]*2,sinv[i]*2);
      glVertex3f(-1,cosv[i]*2,sinv[i]*2);
      glNormal3f(0,cosv[i+1],sinv[i+1]);
      glVertex3f(-1,cosv[i+1]*2,sinv[i+1]*2);
      glVertex3f(1,cosv[i+1]*2,sinv[i+1]*2);
      //capa de adentro
      glNormal3f(0,cosv[i],sinv[i]);
      glVertex3f(1,cosv[i],sinv[i]);
      glVertex3f(-1,cosv[i],sinv[i]);
      glNormal3f(0,cosv[i+1],sinv[i+1]);
      glVertex3f(-1,cosv[i+1],sinv[i+1]);
      glVertex3f(1,cosv[i+1],sinv[i+1]);
      
      // tapa frente
      glNormal3f(-1,0,0);
      glVertex3f(-1,cosv[i]*2,sinv[i]*2);
      glVertex3f(-1,cosv[i+1]*2,sinv[i+1]*2);
      glVertex3f(-1,cosv[i+1],sinv[i+1]);
      glVertex3f(-1,cosv[i],sinv[i]);
      // tapa atras
      glNormal3f(1,0,0);
      glVertex3f(1,cosv[i]*2,sinv[i]*2);
      glVertex3f(1,cosv[i+1]*2,sinv[i+1]*2);
      glVertex3f(1,cosv[i+1],sinv[i+1]);
      glVertex3f(1,cosv[i],sinv[i]);
    }
  glEnd();
  
  static float aux_color_2[] = {.51,.51,.51};
  set_material(MT_Metal,aux_color_2);
  glBegin(GL_QUADS);
    for (int i=0;i<lod;i++) {
      // tapa frente
      int i2=(i+lod)%lod;
      glVertex3f(-.7,cosv[i],sinv[i]);
      glVertex3f(-.7,cosv[i+1],sinv[i+1]);
      glVertex3f(-.7,-cosv[i2],-sinv[i2]);
      glVertex3f(-.7,-cosv[i2],-sinv[i]);
      glVertex3f(.7,cosv[i],sinv[i]);
      glVertex3f(.7,cosv[i+1],sinv[i+1]);
      glVertex3f(.7,-cosv[i2],-sinv[i2]);
      glVertex3f(.7,-cosv[i2],-sinv[i]);
    }
  glEnd();
}

void drawChasis() {
  set_material(MT_Metal,color1);
  glBegin(GL_TRIANGLES);
    glNormal3f(.4,-1,0);
    glVertex3f(-1,-1,-1);
    glVertex3f(-1,-1,1);
    glVertex3f(1,0,0);
    
    glNormal3f(.4,1,0);
    glVertex3f(-1,1,-1);
    glVertex3f(-1,1,1);
    glVertex3f(1,0,0);
    
    glNormal3f(-.2,+.5,1); glVertex3f(-1,1,1);
    glNormal3f(-.2,-.5,1); glVertex3f(-1,-1,1);
    glNormal3f(.4,0,1);    glVertex3f(1,0,0);
    
    glNormal3f(.4,0,-1);
    glVertex3f(-1,1,-1);
    glVertex3f(-1,-1,-1);
    glVertex3f(1,0,0);
  glEnd();
  glBegin(GL_QUADS);
    glNormal3f(-1,0,0);
    glVertex3f(-1,-1,-1);
    glVertex3f(-1,1,-1);
    glVertex3f(-1,1,1);
    glVertex3f(-1,-1,1);
  glEnd();
}

void drawAleron() {
  set_material(MT_Metal,color3);
  glBegin(GL_TRIANGLES);
    glNormal3f(0,-1,0);
    glVertex3f(-1,-1,-1);
    glVertex3f(-1,-1,1);
    glVertex3f(1,-1,-1);
    glNormal3f(0,1,0);
    glVertex3f(-1,1,-1);
    glVertex3f(-1,1,1);
    glVertex3f(1,1,-1);
  glEnd();
  glBegin(GL_QUADS);
    glNormal3f(0.3,-.3,1); glVertex3f(-1,1,1);
    glNormal3f(0.3,+.3,1); glVertex3f(-1,-1,1);
    glNormal3f(0.0,+.3,1); glVertex3f(1,-1,-1);
    glNormal3f(0.0,-.3,1); glVertex3f(1,1,-1);
    
    glNormal3f(-1,0,0);
    glVertex3f(-1,1,-1);
    glVertex3f(-1,-1,-1);
    glVertex3f(-1,-1,1);
    glVertex3f(-1,1,1);
    
    glNormal3f(0,0,-1);
    glVertex3f(-1,1,-1);
    glVertex3f(-1,-1,-1);
    glVertex3f(1,-1,-1);
    glVertex3f(1,1,-1);

  glEnd();
}

void drawCasco(int lod) {
  set_material(MT_Metal,color0);
  glutSolidSphere(1,lod,lod);
}

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
//  glBegin(GL_LINES);
//    glVertex3f(-1,-1,-1); glVertex3f(1,-1,-1);
//    glVertex3f(-1,-1,-1); glVertex3f(-1,1,-1);
//    glVertex3f(-1,-1,-1); glVertex3f(-1,-1,1);
//    glVertex3f(1,1,1); glVertex3f(1,1,-1);
//    glVertex3f(1,1,1); glVertex3f(1,-1,1);
//    glVertex3f(1,1,1); glVertex3f(-1,1,1);
//    glVertex3f(-1,-1,1); glVertex3f(1,-1,1);
//    glVertex3f(-1,-1,1); glVertex3f(-1,1,1);
//    glVertex3f(-1,1,-1); glVertex3f(-1,1,1);
//    glVertex3f(-1,1,-1); glVertex3f(1,1,-1);
//    glVertex3f(1,-1,-1); glVertex3f(1,1,-1);
//    glVertex3f(1,-1,-1); glVertex3f(1,-1,1);
//  glEnd();
  
  // plano para apoyar el auto
  static float aux_color_2[] = {.5,.5,.5};
  set_material(MT_Glass,aux_color_2);
  glBegin(GL_QUADS);
    glVertex3f(-1,-1,0);
    glVertex3f(-1, 1,0);
    glVertex3f( 1, 1,0);
    glVertex3f( 1,-1,0);
  glEnd();
}

void drawToma() {    
  set_material(MT_Metal,color2);
  glBegin(GL_QUADS);
    glNormal3f(0,0,-1);
    glVertex3f(-1,-1,-1);
    glVertex3f(-1,1,-1);
    glVertex3f(1,1,-1);
    glVertex3f(.3,-.3,-1);
    
    glNormal3f(-.2,-.4,.7); glVertex3f(-1, -1, 1);
    glNormal3f(-.2,  0,.7); glVertex3f(-1,  1, 1);
    glNormal3f(+.4,  0,.7); glVertex3f( 1,  1,.3);
    glNormal3f(+.4,-.4,.7); glVertex3f(.3,-.3,.3);
    
    glNormal3f(.3,-1,0);
    glVertex3f(-1,-1,-1);
    glVertex3f(-1,-1,1);
    glVertex3f(.3,-.3,.3);
    glVertex3f(.3,-.3,-1);
  glEnd();
}

void drawPista(int w, int h) {
  const int w_2 = w*3/2, h_2=h*3/2;
  set_material(MT_Texture);
  glBegin(GL_QUADS);
    glNormal3f(0,0,1);
    glTexCoord2d(0,3); glVertex2f(-w_2,h_2);
    glTexCoord2d(3,3); glVertex2f(w_2,h_2);
    glTexCoord2d(3,0); glVertex2f(w_2,-h_2);
    glTexCoord2d(0,0); glVertex2f(-w_2,-h_2);
  glEnd();
}

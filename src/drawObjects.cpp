#include <cmath> // sin cos
#ifdef __APPLE__
# include <OpenGL/gl.h>
#else
# include <GL/gl.h>
#endif
#include "Auto.hpp"
#include "drawObjects.hpp"
#include "drawPartes.hpp"
#include "Malla.hpp"
#include <iostream>
using namespace std;

void drawPez(bool relleno, Malla *malla) {
  
  glPushMatrix();
  glScaled(0.25,0.25,0.25);
  glRotated(90,0,0,1);
  glRotated(90,1,0,0);
  malla->Draw(relleno);
  glPopMatrix();
  
}

void drawObjects(bool animado, bool relleno, Spline curva, Malla *malla) {
  
  if (animado) {
    /// @TODO: ubicar el auto en la pista
    glPushMatrix();
    glTranslatef(el_pez.x,el_pez.y,el_pez.z);
    glRotatef(el_pez.ang_xy,0,0,1);
    glRotatef(el_pez.ang_xz,0,1,0);
    drawPez(relleno, malla);
    glPopMatrix();
  }
  if (!animado) {    
    
    glPushMatrix();
    glScalef(0.1,0.1,0.1);
    curva.Dibujar(10,true);
    glPopMatrix();
    
    drawPez(relleno, malla);
    
    drawCube();
    drawEjes();
  }
}

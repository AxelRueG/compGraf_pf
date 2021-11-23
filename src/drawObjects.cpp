#include <cmath> // sin cos
#ifdef __APPLE__
# include <OpenGL/gl.h>
#else
# include <GL/gl.h>
#endif
#include "Auto.hpp"
#include "drawObjects.hpp"
#include "drawPartes.hpp"

#define mat_error DEBEN_ARMAR_LAS_MATRICES_MANUALMENTE
#define glRotated(a,x,y,z) mat_error
#define glRotatef(a,x,y,z) mat_error
#define glTranslatef(x,y,z) mat_error
#define glTranslated(x,y,z) mat_error
#define glScaled(x,y,z) mat_error
#define glScalef(x,y,z) mat_error
#include "Malla.hpp"
#include <iostream>
using namespace std;


/// ejemplo de sintaxis (nota: la matriz se escribe "transpuesta" respecto a como la escribimos normalmente)
///  glPushMatrix();
///    float mt[] = { ex_x, ex_y, ex_z, 0.00,  
///                   ey_x, ey_y, ey_z, 0.00,  
///                   ez_x, ez_y, ez_z, 0.00,  
///                    dx ,  dy ,  dz , 1.00 };
///    glMultMatrixf(m);
///    drawSomething();
///  glPopMatrix();


//void drawAuto(int lod) {
//  
//  /// @TODO: aplicar las transformaciones necesarias para ubicar las partes del auto como van
//  
//  
//  
//}

void drawObjects(bool animado, Malla *malla) {
  
  if (animado) {
    /// @TODO: ubicar el auto en la pista
  }
  
  malla->Draw(true);
  
  if (!animado) {
    drawCube();
    drawEjes();
  }
  
}

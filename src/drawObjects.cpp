#include <cmath> // sin cos
#ifdef __APPLE__
# include <OpenGL/gl.h>
#else
# include <GL/gl.h>
#endif
#include "Pez.hpp"
#include "drawObjects.hpp"
#include "drawPartes.hpp"
#include "Malla.hpp"
#include <iostream>
#include <cmath>
using namespace std;

static float n=0;

void drawPez(bool relleno, Malla *malla) {
  
  glPushMatrix();
  glScaled(0.25,0.25,0.25);
  glRotated(90,0,0,1);
  glRotated(90,1,0,0);
  malla->Draw(relleno);
  glPopMatrix();
  
}

void drawObjects(bool animado, bool relleno, Spline curva, Malla *malla) {
  	
  if (animado) { /// ubicar el pez en el agua
    
	if (curva.ocultar) curva.Dibujar(10,false);  
    
	glPushMatrix();
	glTranslatef(el_pez.x,el_pez.y,el_pez.z);

	if(el_pez.pezZigZaguea){
		constexpr float R2G = 180.f/3.141516; // radianes a grados
		float theta = el_pez.t*360;
		if (n<6.28){
			glRotatef((0.3*(cos(n)))*R2G,1,0,1);
			n+=0.1;
			if(n>6.28){
				n=0.f;
			}
		}
	}
	
	glRotatef(el_pez.ang_zx,0,1,0);
	glRotatef(el_pez.ang_xy,0,0,1);
	drawPez(relleno, malla);
	glPopMatrix();
  }
  if (!animado) { //apoyado sobre el plano junto a la curva representada
    
    glPushMatrix();
    glScalef(0.1,0.1,0.1);
	if (curva.ocultar) curva.Dibujar(10,true);
    glPopMatrix();
    
    drawPez(relleno,malla);
    
    drawCube();
    drawEjes();
  }
}

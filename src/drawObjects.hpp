#ifndef DRAWOBJECTS_H
#define DRAWOBJECTS_H
#include "Malla.hpp"
#include "Curva.h"

void drawObjects(bool animado); // solo dibuja los ejes si esta sin animar
void drawObjects(bool animado, bool relleno, Malla *malla); // dibuja la malla
void drawObjects(bool animado, Spline curva); // dibuja la malla

#endif

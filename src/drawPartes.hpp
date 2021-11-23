#ifndef DRAW_PARTES_H
#define DRAW_PARTES_H

// dibuja la forma que se utiliza para la parte principal y más grande del chasis
// y también, a distinta escala y con diferente color, para la parte que va justo 
// detrás de la cabeza del piloto
void drawChasis();

// dibuja una rueda, con su eje de giro coincidiendo con el eje z
void drawRueda(int lod=10);

// función para dibujar un aleron... con diferente escala se usa tanto para
// el trasero como el delantero
void drawAleron();

// función para dibujar el casco del piloto
void drawCasco(int lod);

// dibuja las partes grandes laterales del chasis
void drawToma();

// dibuja el plano de la pista con su correspondiente textura
void drawPista(int w, int h);

// función que dibuja los ejes del sistema de coordenadas... cada eje es un 
// segmento de 1 unidad de largo que parte del origen del sistema... los
// colores R, G, B se corresponden con lo ejes X, Y, Z respectivamente
void drawEjes();

// dibuja el cubo de referencia que va desde -1 a +1 en todas las coordenadas,
// y dentro un plano a la altura z=0 que sirve para tener una referencia la 
// posición de la pista respecto al auto
void drawCube();


#endif


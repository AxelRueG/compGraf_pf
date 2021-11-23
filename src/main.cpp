// Transformaciones (buscar @TODO y ver consignas.pdf)

#include <cmath> // atan sqrt
#include <cstdlib> // exit
#include <fstream> // file io
#include <iostream>
#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <GLUT/glut.h>
#else
# include <GL/gl.h>
# include <GL/glut.h>
#endif
#include "Auto.hpp"
#include "drawObjects.hpp"
#include "load_texture.hpp"
#include "OSD.hpp"
#include "Malla.hpp"

//------------------------------------------------------------
// variables globales y defaults
Malla *malla;

int
  w=800,h=600, // tamaño de la ventana
  boton=-1, // boton del mouse clickeado
  xclick,yclick, // x e y cuando clickeo un boton
  lod=10; // nivel de detalle (subdivisiones de lineas y superficies parametricas)
float // luces y colores en float
  lpos[]={2,10,5,0}, // posicion luz, l[4]: 0 => direccional -- 1 => posicional
  escala=125,escala0, // escala de los objetos window/modelo pixeles/unidad
  dist_cam=4, // distancia del ojo al origen de coordenadas en la manipulación
  eye[]={.5,.5,.5}, target[]={0,0,0}, up[]={0,0,1}, // camara, mirando hacia y vertical
  znear=1, zfar=50, //clipping planes cercano y alejado de la camara (en 5 => veo de 3 a -3)
  amy,amy0, // angulo del modelo alrededor del eje y
  ac0=1,rc0, // angulo resp x y distancia al target de la camara al clickear
  sky_color[]={.4,.4,.8}, // color del fondo (azul)
  back_color[]={0,.2,0,1};

bool // variables de estado de este programa
  animado=false,    // el auto se mueve por la pista
  dibuja=true,      // false si esta minimizado
  relleno=true,     // dibuja relleno o no
  top_view=false;    // vista superior o vista trasera del auto cuando animate=true

short modifiers=0;  // ctrl, alt, shift (de GLUT)
inline short get_modifiers() {return modifiers=(short)glutGetModifiers();}

// temporizador:
static int msecs=20; // milisegundos por frame

// para saber qué teclas hay apretadas cuando se calcula el movimiento del auto
static int keys[4]; // se modifica Special_cb y SpecialUp_cb, se usa en Idle_cb

bool init_texture() {
  static bool done = false, ok = false;
  if (done) return ok; done = true;
  
  GLint max_texture_size=0; 
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
  const char *texture_file = "pista_1024.png";
  if      (max_texture_size>=4096) texture_file = "pista_4096.png" ; 
  else if (max_texture_size>=2048) texture_file = "pista_2048.png" ; 
  
  OSD << "Cargando " << texture_file << "..."; OSD.Render(w,h); glutSwapBuffers();
  Texture tex_pista = load_texture(texture_file,true);
  if (!tex_pista.data) return ok=false;
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  return ok = true;
}


//------------------------------------------------------------
// redibuja los objetos cada vez que hace un redisplay
void Display_cb() { // Este tiene que estar
  if (!dibuja) return;
  
  // borra los buffers de pantalla y z
  if (animado) glClearColor(sky_color[0],sky_color[1],sky_color[2],1);
  else         glClearColor(back_color[0],back_color[1],back_color[2],1);
  glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
  glPolygonMode(GL_FRONT_AND_BACK,relleno?GL_FILL:GL_LINE);
  
  if (animado && !init_texture()) OSD << "ERROR: NO SE PUDO CARGAR LA TEXTURA DE LA PISTA!\n\n";

  glPushMatrix();
  
  /// @TODO: Esta luz se mueve con la camara... no debería
  glLightfv(GL_LIGHT0,GL_POSITION,lpos);  // ubica la luz
  
  if (animado) {
    
    if (top_view) {
      
      /// @TODO: Hacer que la camara gire para que el auto siempre apunte hacia arriba
      gluLookAt(el_auto.x,el_auto.y,15,el_auto.x,el_auto.y,0,1,0,0);
      
    } else {
      
      OSD << "FALTA IMPLEMENTAR EL LOOKAT PARA ESTA VISTA" << '\n';
      /// @TODO: Ubicar correctamente la camara con gluLookAt, (y ver qué pasa con la luz cuando el auto se meuve)
      
    }
    
  } else {
    
    OSD << "Presione T o R para correr" << '\n';
    gluLookAt(dist_cam*eye[0],dist_cam*eye[1],dist_cam*eye[2],0,0,0,up[0],up[1],up[2]);
  
  }
  
  drawObjects(animado,malla);
  glPopMatrix();

#ifdef _DEBUG
  OSD << "el_auto {\n"
      << "  x: " << el_auto.x << '\n'
      << "  y: " << el_auto.y << '\n'
      << "  vel: " << el_auto.vel << '\n'
      << "  ang: " << el_auto.ang << '\n'
      << "  rang1: " << el_auto.rang1 << '\n'
      << "  rang2: " << el_auto.rang2 << '\n'
      << "}\n";
  OSD << "l.o.d.: " << lod<<'\n';
//  OSD << "escala: " << escala <<'\n';
  OSD << "vista: " << (animado?(top_view?"superior":"trasera"):"cubo") <<'\n';
#endif
  
  glColor3f(1,1,1);
  OSD.Render(w,h);
  
  glutSwapBuffers();

#ifdef _DEBUG
  // chequea errores
  for(int errornum=glGetError();errornum!=GL_NO_ERROR;errornum=glGetError()) {
    if(errornum==GL_INVALID_ENUM)
      OSD << "OpenGL error: GL_INVALID_ENUM" << '\n';
    else if(errornum==GL_INVALID_VALUE)
      OSD << "OpenGL error: GL_INVALID_VALUE" << '\n';
    else if (errornum==GL_INVALID_OPERATION)
      OSD << "OpenGL error: GL_INVALID_OPERATION" << '\n';
    else if (errornum==GL_STACK_OVERFLOW)
      OSD << "OpenGL error: GL_STACK_OVERFLOW" << '\n';
    else if (errornum==GL_STACK_UNDERFLOW)
      OSD << "OpenGL error: GL_STACK_UNDERFLOW" << '\n';
    else if (errornum==GL_OUT_OF_MEMORY)
      OSD << "OpenGL error: GL_OUT_OF_MEMORY" << '\n';
  }
#endif // _DEBUG
}

//------------------------------------------------------------
// Regenera la matriz de proyeccion
// cuando cambia algun parametro de la vista
void regen() {
  if (!dibuja) return;

  // matriz de proyeccion
  glMatrixMode(GL_PROJECTION);  glLoadIdentity();

  double w0=(double)w/2/escala,h0=(double)h/2/escala; // semiancho y semialto en el target

  // frustum, pal eye pero con los z positivos (delante del ojo)
  w0*=znear/dist_cam,h0*=znear/dist_cam;  // w0 y h0 en el near
  glFrustum(-w0,w0,-h0,h0,znear,zfar);

  glMatrixMode(GL_MODELVIEW); glLoadIdentity(); // matriz del modelo->view

  glutPostRedisplay(); // avisa que hay que redibujar
}

//------------------------------------------------------------
// Animacion

// Cuando no hay ningún otro evento se invoca   a glutIdleFunc 
// El "framerate" lo determina msec, a menos que la complejidad 
// del modelo (lod) y la no aceleracion por hardware lo bajen
void Idle_cb() {
  static int anterior=glutGet(GLUT_ELAPSED_TIME); // milisegundos desde que arranco

  int tiempo=glutGet(GLUT_ELAPSED_TIME), lapso=tiempo-anterior;
  // esperar 1/60 segundos antes de pasar al próximo cuadro
  if (lapso<=16) { return; }
  
  anterior=tiempo;
  
  float acel = (+1)*keys[0] + (-1)*keys[1];
  float dir  = (+1)*keys[2] + (-1)*keys[3];
  el_auto.Mover(acel,dir);
  
  glutPostRedisplay();
}

//------------------------------------------------------------
// Maneja cambios de ancho y alto de la ventana
void Reshape_cb(int width, int height){
  h=height; w=width;

  if (w==0||h==0) {// minimiza
    dibuja=false; // no dibuja mas
    glutIdleFunc(0); // no llama a cada rato a esa funcion
    return;
  }
  else if (!dibuja&&w&&h){// des-minimiza
    dibuja=true; // ahora si dibuja
    glutIdleFunc(Idle_cb); // registra de nuevo el callback
  }

  glViewport(0,0,w,h); // region donde se dibuja

  regen(); //regenera la matriz de proyeccion
}

//------------------------------------------------------------
// Movimientos del mouse

// Rotacion del punto (x,y,z) sobre el eje (u,v,w), formula de Rodriguez
void Rotate(float &x, float &y ,float &z, float u, float v, float w, float a) {
  float ux=u*x, uy=u*y, uz=u*z;
  float vx=v*x, vy=v*y, vz=v*z;
  float wx=w*x, wy=w*y, wz=w*z;
  float sa=sin(a), ca=cos(a);
  x=u*(ux+vy+wz)+(x*(v*v+w*w)-u*(vy+wz))*ca+(-wy+vz)*sa;
  y=v*(ux+vy+wz)+(y*(u*u+w*w)-v*(ux+wz))*ca+(wx-uz)*sa;
  z=w*(ux+vy+wz)+(z*(u*u+v*v)-w*(ux+vy))*ca+(-vx+uy)*sa;
}

void Normalize(float x[]) {
  float m=sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]);
  x[0]/=m; x[1]/=m; x[2]/=m;
}

// girar la camara o hacer zoom
void Motion_cb(int xm, int ym){ // drag
  if (modifiers==GLUT_ACTIVE_SHIFT) { // cambio de escala
    escala=escala0*exp((yclick-ym)/100.0);
    regen();
  } else  { // manipulacion
    /// @TODO: Analizar como se giran los vectores eye y up
    float angulox=double(xclick-xm)/100; // respecto a up
    Rotate(eye[0],eye[1],eye[2],up[0],up[1],up[2],angulox);
    float anguloy=double(ym-yclick)/100; // respecto a up vectorial eye
    float // up vectorial eye normalizado
      vx=((up[2]*eye[1])-(up[1]*eye[2])),
      vy=((up[0]*eye[2])-(up[2]*eye[0])),
      vz=((up[1]*eye[0])-(up[0]*eye[1]));
    Rotate(eye[0],eye[1],eye[2],vx,vy,vz,anguloy);
    Rotate(up[0],up[1],up[2],vx,vy,vz,anguloy);
    Normalize(eye); Normalize(up); // mata el error numérico
    xclick=xm; yclick=ym;
    glutPostRedisplay();
  }
}

// Clicks del mouse
// GLUT LEFT BUTTON, GLUT MIDDLE BUTTON, or GLUT RIGHT BUTTON
// The state parameter is either GLUT UP or GLUT DOWN
// glutGetModifiers may be called to determine the state of modifier keys
void Mouse_cb(int button, int state, int x, int y){
  if (button==GLUT_LEFT_BUTTON){
    if (state==GLUT_DOWN) {
      xclick=x; yclick=y;
      boton=button;
      get_modifiers();
      glutMotionFunc(Motion_cb);
      if (modifiers==GLUT_ACTIVE_SHIFT) escala0=escala; // escala      
      else { // manipulacion
        double yc=eye[1]-target[1],zc=eye[2]-target[2];
        rc0=sqrt(yc*yc+zc*zc); ac0=atan2(yc,zc);
        amy0=amy;
      }
    }
    else if (state==GLUT_UP){
      boton=-1;
      glutMotionFunc(NULL);
    }
  }
}
//------------------------------------------------------------
// Teclado
// Special keys (non-ASCII)
// aca es int key
void Special_cb(int key,int xm=0,int ym=0) {
       if (key==GLUT_KEY_UP)    keys[0]=1;
  else if (key==GLUT_KEY_DOWN)  keys[1]=1;
  else if (key==GLUT_KEY_LEFT)  keys[2]=1;
  else if (key==GLUT_KEY_RIGHT) keys[3]=1;
}

void SpecialUp_cb(int key,int xm=0,int ym=0) {
  if (key==GLUT_KEY_UP)         keys[0]=0;
  else if (key==GLUT_KEY_DOWN)  keys[1]=0;
  else if (key==GLUT_KEY_LEFT)  keys[2]=0;
  else if (key==GLUT_KEY_RIGHT) keys[3]=0;
}

// Maneja pulsaciones del teclado (ASCII keys)
void Keyboard_cb(unsigned char key,int x=0,int y=0) {
  switch (key){
    case 'f': case 'F': // relleno
      relleno=!relleno;
      if (relleno) 
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      else
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      break;
    case 'r': case 'R': // movimiento
      animado=!animado;
      break;
    case 't': case 'T': // movimiento
      if (!animado) Keyboard_cb('r',x,y);
      top_view=!top_view;
      break;
    case 27: // escape => exit
      get_modifiers();
      if (!modifiers)
        exit(EXIT_SUCCESS);
      break;
    case '+': case '-': // lod
      if (key=='+') {
        lod++; 
//        malla->Subdivide(); // subdivicon catmull para superficies
      }
      else {
        lod--; 
        if (lod==0) lod=1;
      }
      break;
  }
  regen();
  glutPostRedisplay();
}

//------------------------------------------------------------
// Menu
void Menu_cb(int value)
{
  switch (value){
    case 'n':
      Keyboard_cb('n');
      return;
    case 'f':
      Keyboard_cb('f');
      return;
    case 'i':
      Keyboard_cb('i');
      return;
    case 'p':
      Keyboard_cb('p');
      return;
    case 'r':
      Keyboard_cb('r');
      return;
    case 't':
      Keyboard_cb('t');
      return;
    case '+':
      Keyboard_cb('+');
      return;
    case '-':
      Keyboard_cb('-');
      return;
    case 27: //esc
      exit(EXIT_SUCCESS);
  }
}

//------------------------------------------------------------
// Inicializa GLUT y OpenGL
void initialize() {
  // pide z-buffer, color RGB y double buffering
  glutInitDisplayMode(GLUT_DEPTH|GLUT_RGB|GLUT_ALPHA|GLUT_DOUBLE);

  glutInitWindowSize(w,h); glutInitWindowPosition(50,50);

  glutCreateWindow("Transformaciones-F1"); // crea el main window

  //declara los callbacks
  //los que no se usan no se declaran
  glutDisplayFunc(Display_cb); // redisplays
  glutReshapeFunc(Reshape_cb); // cambio de alto y ancho
  glutKeyboardFunc(Keyboard_cb); // teclado
  glutSpecialFunc(Special_cb); // teclas especiales (cuando se apretan)
  glutSpecialUpFunc(SpecialUp_cb); // teclas especiales (cuando se sueltan)
  glutMouseFunc(Mouse_cb); // botones picados
  glutIdleFunc(Idle_cb); // registra el callback

  // crea el menu
  glutCreateMenu(Menu_cb);
    glutAddMenuEntry("   [f]_Caras Rellenas        ", 'f');
    glutAddMenuEntry("   [r]_Anima                 ", 'r');
    glutAddMenuEntry("   [t]_Vista Superior        ", 't');
    glutAddMenuEntry("   [+]_+Nivel de Detalle     ", '+');
    glutAddMenuEntry("   [-]_-Nivel de Detalle     ", '-');
    glutAddMenuEntry(" [Esc]_Exit                  ", 27);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  // ========================
  // estado normal del OpenGL
  // ========================

  glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL); // habilita el z-buffer
  glEnable(GL_NORMALIZE); // para que el scaling no moleste
  glEnable(GL_AUTO_NORMAL); // para nurbs??
  glEnable(GL_POLYGON_OFFSET_FILL); glPolygonOffset (1,1); // coplanaridad

  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  // direccion de los poligonos
  glFrontFace(GL_CCW); glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glCullFace(GL_BACK); glDisable(GL_CULL_FACE);// se habilita por objeto (cerrado o abierto)

  // define luces
  static const float
    lambient[]={.4f,.4f,.4f,1.f}, // luz ambiente
    ldiffuse[]={.7f,.7f,.7f,1.f}, // luz difusa
    lspecular[]={1.f,1.f,1.f,1.f};      // luz especular
  glLightfv(GL_LIGHT0,GL_AMBIENT,lambient);
  glLightfv(GL_LIGHT0,GL_DIFFUSE,ldiffuse);
  glLightfv(GL_LIGHT0,GL_SPECULAR,lspecular);
  // caras de atras y adelante distintos (1) o iguales (0)
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,0);
  glEnable(GL_LIGHT0);
  glShadeModel(GL_SMOOTH);

  // material y textura
  glEnable(GL_COLOR_MATERIAL);

  regen(); // para que setee las matrices antes del 1er draw
}

//------------------------------------------------------------
// main
int main(int argc,char** argv) {
  glutInit(&argc,argv);// inicializa glut
  malla = new Malla("star.dat");  
  initialize(); // condiciones iniciales de la ventana y OpenGL
  glutMainLoop(); // entra en loop de reconocimiento de eventos
  return 0;
}

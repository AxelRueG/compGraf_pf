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
#include "Pez.hpp"
#include "drawObjects.hpp"
#include "load_texture.hpp"
#include "OSD.hpp"
#include "Malla.hpp"
#include "Curva.h"

//------------------------------------------------------------
// variables globales y defaults
Malla *malla;
Spline curva;
punto d; //derivada
bool ocultar = true;
bool pezNada = true;
int curvaN = 1; //para saber que curva se esta mostrando

struct PuntosDeControl {
  float x=0,y=0,z=0;
};

static PuntosDeControl Curva1[9] = {
	{ 10.f,   0.f},
	{  5.f,   5.f},
	{  0.f,  10.f},
	{ -5.f,   5.f},
	{-10.f,   0.f},
	{ -5.f,  -5.f},
	{  0.f, -10.f},
	{  5.f,  -5.f},
	{ 10.f,   0.f}	
};
static PuntosDeControl Curva2[12] = {
	{  0.f,   0.f,  1.f},
	{ 10.f,   0.f,  2.f},
	{  0.f,  10.f,  3.f},
	{-10.f,   0.f,  4.f},
	{  0.f, -10.f,  5.f},
	{  0.f,   0.f,  6.f},
	{ 10.f,   0.f,  7.f},
	{  0.f,  10.f,  8.f},
	{-10.f,   0.f,  9.f},
	{  0.f, -10.f, 10.f},
	{  0.f,   0.f, 11.f},
	{ 10.f,   0.f, 12.f}
};
static PuntosDeControl Curva3[8] = {
	{ 7.f,   4.f,   4.4},
	{-7.6,   3.f,   2.2},
	{ 6.f,  -2.5,  -8.5},
	{-3.5,   9.2,   2.3},
	{-8.1,  -1.f,  -4.4},
	{ 1.f,  -8.3,   2.7},
	{ 9.f,  -1.1,   1.1},
	{ 7.f,   4.f,   4.4}
};


int
  w=800,h=600, // tamaï¿½o de la ventana
  boton=-1, // boton del mouse clickeado
  xclick,yclick, // x e y cuando clickeo un boton
  lod=10; // nivel de detalle (subdivisiones de lineas y superficies parametricas)
float // luces y colores en float
  lpos[]={2,15,10,0}, // posicion luz, l[4]: 0 => direccional -- 1 => posicional
  escala=125,escala0, // escala de los objetos window/modelo pixeles/unidad
  dist_cam=4, // distancia del ojo al origen de coordenadas en la manipulaciï¿½n
  eye[]={.5,.5,.5}, target[]={0,0,0}, up[]={0,0,1}, // camara, mirando hacia y vertical
  znear=1, zfar=50, //clipping planes cercano y alejado de la camara (en 5 => veo de 3 a -3)
  amy,amy0, // angulo del modelo alrededor del eje y
  ac0=1,rc0, // angulo resp x y distancia al target de la camara al clickear
  sky_color[]={.4,.4,.8}, // color del fondo (azul)
  back_color[]={0,0.2,0.4,1};

bool // variables de estado de este programa
  animado=false,    // el auto se mueve por la pista
  dibuja=true,      // false si esta minimizado
  relleno=true,     // dibuja relleno o no
  top_view=false;    // vista superior o vista trasera del auto cuando animate=true

short modifiers=0;  // ctrl, alt, shift (de GLUT)
inline short get_modifiers() {return modifiers=(short)glutGetModifiers();}

// temporizador:
static int msecs=20; // milisegundos por frame

// para saber que teclas hay apretadas cuando se calcula el movimiento del auto
static int keys[4]; // se modifica Special_cb y SpecialUp_cb, se usa en Idle_cb


/// ---------------------------------------------------------------------------------
///								    TEXTURA DE PEZ
/// ---------------------------------------------------------------------------------

GLuint texid;

bool init_texture() {
	static bool done = false, ok = false;
	if (done) return ok; done = true;
	
	//GLint max_texture_size=0; 
	//glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
	const char *texture_file = "clown2.png";
	
	OSD << "Cargando " << texture_file << "..."; OSD.Render(w,h); glutSwapBuffers();
	Texture tex_pez = load_texture(texture_file,true);
	//cout<<tex_pez.data<<endl;
	if (!tex_pez.data) return ok=false;
	
	// color material
	glColor3f(1.f,1.f,1.f);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
	glEnable(GL_TEXTURE_2D);
	
	return ok = true;
}

/// ---------------------------------------------------------------------------------
/// ---------------------------------------------------------------------------------

//------------------------------------------------------------
// redibuja los objetos cada vez que hace un redisplay
void Display_cb() { // Este tiene que estar
  if (!dibuja) return;
  
  // borra los buffers de pantalla y z
  if (animado) glClearColor(sky_color[0],sky_color[1],sky_color[2],1);
  else         glClearColor(back_color[0],back_color[1],back_color[2],1);
  glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
  glPolygonMode(GL_FRONT_AND_BACK,relleno?GL_FILL:GL_LINE);
  	
//  if (animado && !init_texture()) OSD << "ERROR: NO SE PUDO CARGAR LA TEXTURA DE LA PISTA!\n\n";
  
  glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
  glPolygonMode(GL_FRONT_AND_BACK,relleno?GL_FILL:GL_LINE);
  
  // ubicamos la luz para que se transforme junto al espacio
  glLightfv(GL_LIGHT0,GL_POSITION,lpos); 
  
  glPushMatrix();
  
  if (animado) {
	  
	if (top_view) {
		gluLookAt(-15*eye[0],-15*eye[1],-15*eye[2],0,0,0,up[0],up[1],up[2]);
    } else {
		//ACA CAMBIÉ EL VECTOR NORMAL PARA PODER VER EL PEZ DE ARRIBA
		gluLookAt(el_pez.x,el_pez.y,el_pez.z+15,el_pez.x,el_pez.y,el_pez.z,1,0,0);
    }
    
  } else {
    OSD << "Presione T o R para que el pez nade" << '\n';
    gluLookAt(dist_cam*eye[0],dist_cam*eye[1],dist_cam*eye[2],0,0,0,up[0],up[1],up[2]);
  }  
  /// AQUI ESTOY DIBUJANDO EL PEZ
  if (el_pez.pezNada) malla->MoveMalla();
  
  drawObjects(animado,relleno,curva,malla);
  glPopMatrix();
  //glutSwapBuffers();

  #ifdef _DEBUG
    OSD << "el_pez {\n"
        << "  x: " << el_pez.x << '\n'
        << "  y: " << el_pez.y << '\n'
        << "  vel: " << el_pez.z << '\n'
        << "  ang_xy: " << el_pez.ang_xy << '\n'
        << "  ang_xz: " << el_pez.ang_xz << '\n'
        << "  t: " << el_pez.t << '\n'
        << "}\n";
    OSD << "l.o.d.: " << lod<<'\n';
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
// Regenera la matriz de proyeccion cuando cambia algun parametro de la vista
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

// Cuando no hay ningun otro evento se invoca a glutIdleFunc 
// El "framerate" lo determina msec, a menos que la complejidad 
// del modelo (lod) y la no aceleracion por hardware lo bajen
void Idle_cb() {
  static int anterior=glutGet(GLUT_ELAPSED_TIME); // milisegundos desde que arranco

  int tiempo=glutGet(GLUT_ELAPSED_TIME), lapso=tiempo-anterior;
  // esperar 1/60 segundos antes de pasar al proximo cuadro
  if (lapso<=16) { return; }
  
  anterior=tiempo;
  
  float acel = (+1)*keys[0] + (-1)*keys[1];
  float dir  = (+1)*keys[2] + (-1)*keys[3];
  // tenemos aca como se mueve el pez no es del todo funcional
  el_pez.Mover(curva);
  
  
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
    Normalize(eye); Normalize(up); // mata el error numï¿½rico
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
	case 'o': case 'O': // movimiento
	  curva.ocultar=!curva.ocultar;
	  break;
	case 'a': case 'A': // movimiento
		el_pez.pezNada=!el_pez.pezNada;
	  break;
    case 'z': case 'Z': // movimiento
	  el_pez.pezZigZaguea=!el_pez.pezZigZaguea;
	  break;  
	// >>>>>>>>>>>>>>>>>> cambio de curva <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	case '1': // Se genera curva1
	  curvaN=1;
	  while(curva.CantPuntos()!=0){
		  curva.Quitar(0);
	  }
	  // cargo los puntos de control
	  for (PuntosDeControl p: Curva1) 
		  curva.Agregar(punto(p.x,p.y,p.z));
	  break;
	case '2': // Se genera curva2
	  curvaN=2;
	  while(curva.CantPuntos()!=0){
		  curva.Quitar(0);
	  }
	  // cargo los puntos de control
	  for (PuntosDeControl p: Curva2) 
		  curva.Agregar(punto(p.x,p.y,p.z));
	  break;
	case '3': // Se genera curva3
	  curvaN=3;
	  while(curva.CantPuntos()!=0){
		  curva.Quitar(0);
	  }
	  // cargo los puntos de control
	  for (PuntosDeControl p: Curva3) 
		  curva.Agregar(punto(p.x,p.y,p.z));
	  break;
	// >>>>>>>>>>>>>>>>>> EXIT <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    case 27: // escape => exit
      get_modifiers();
      if (!modifiers)
        exit(EXIT_SUCCESS);
      break;
    case '+': case '-': // lod
      if (key=='+') {
        lod++; 
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
	case 'o':
	  Keyboard_cb('o');
	  return;
	case 'a':
	  Keyboard_cb('a');
	  return;
    case 'z':
	  Keyboard_cb('z');
	  return;  
	case '1':
	  Keyboard_cb('1');
	  return;
	case '2':
	  Keyboard_cb('2');
	  return;
	case '3':
	  Keyboard_cb('3');
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

  glutCreateWindow("TP_FINAL_PEZ"); // crea el main window

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
    glutAddMenuEntry("   [o]_Mostrar Curva         ", 'o');
    glutAddMenuEntry("   [r]_Anima                 ", 'r');
    glutAddMenuEntry("   [a]_Pez nada              ", 'a');
    glutAddMenuEntry("   [z]_Pez zigzagea          ", 'z');
    glutAddMenuEntry("   [t]_Vista Superior        ", 't');
    glutAddMenuEntry("   [1]_Curva plana           ", '1');
    glutAddMenuEntry("   [2]_Curva espiral         ", '2');
    glutAddMenuEntry("   [3]_Curva random          ", '3');
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

  /// ---------------------------------------------------------------------------------
  ///								    MATERIAL Y TEXTURA
  /// ---------------------------------------------------------------------------------
  
  // material estandar
  float 
	front_color[]={1.f,1.f,1.f,1.f},    // color de caras frontales
	back_color[]={1.f,1.f,1.f,1.f},     // color de caras traseras
	white[]={1.f,1.f,1.f,1.f};          // brillo blanco
  glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,front_color);
  glMaterialfv(GL_BACK,GL_AMBIENT_AND_DIFFUSE,back_color);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
  glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,127);
  
  // textura
  bool cargando1=init_texture();
  if(cargando1) cout<<"Paso la textura"<<endl;
  glEnable(GL_TEXTURE_2D);
  GLfloat plano_s[4] = {0, 0, 0.12, -0.4}; // s=x
  GLfloat plano_t[4] = {0, 0.12, 0, 0}; // t=y
  glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
  glTexGenfv (GL_S, GL_OBJECT_PLANE, plano_s);
  glEnable (GL_TEXTURE_GEN_S);
  glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
  glTexGenfv (GL_T, GL_OBJECT_PLANE, plano_t);
  glEnable (GL_TEXTURE_GEN_T);
 
  regen(); // para que setee las matrices antes del 1er draw

  // cargo los puntos de control
  for (PuntosDeControl p: Curva1) 
	  curva.Agregar(punto(p.x,p.y,p.z));
  
  // los primeros puntos 
  el_pez.x = Curva1[0].x;
  el_pez.y = Curva1[0].y;
  el_pez.z = Curva1[0].z;
}

//------------------------------------------------------------
// main
int main(int argc,char** argv) {
  glutInit(&argc,argv);// inicializa glut
//malla = new Malla("cubo.dat");  
  malla = new Malla("pez.dat");  
  initialize(); // condiciones iniciales de la ventana y OpenGL
  glutMainLoop(); // entra en loop de reconocimiento de eventos
  return 0;
}

//--------------------------------------------------------------------------------------------
//
// File: 	scene.hpp
// Author:	P. Katarzynski (CCE)
//
// Description: Plik naglowkowy dla klasy OpenGL
//
//--------------------------------------------------------------------------------------------

#ifndef _scene_h

#define _scene_h

#include "common.h"
#include "globject.h"
#include "mesh.h"
#include "texture.h"
#include "printer.h"
#include <windows.h>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"


//----------------------------------- DEFINES -------------------------------------------------
#define ESCAPE 27
#define PLAYERHEIGHT 2.0f

//--------------------------------------------------------------------------------------------
// 								CLASS INTERFACE
//--------------------------------------------------------------------------------------------
class Scene
{

public:

Scene(int new_width,int new_height); //  domyslny konstruktor
~Scene(); // domyslny destruktor 

void Draw(); // rysuje zawartosc sceny

void Resize(int new_width, int new_height); // zmienia rozmiar sceny 

void Init(); // procedura inicjujaca przetwarzanie

void Animate(); // przeprowadza animacje sceny 
void SaveAsBmp(char* filename); //zapisuje zrzut ekranu

void KeyDown(unsigned char key); // obsluga klawiatury
void KeyUp(unsigned char key); // obsluga klawiatury

void MouseDown(int x, int y);
void CameraMove(float x, float y);
void MouseWheel(int x, int y);

void Move();
void CheckBorder(glm::vec2 corner, glm::vec2 corner2);
void CheckCollision(glObject * Object);

bool fw, bw, lt, rt, fast, jump, rise, crouch, crouch_handle, crouch_up, freeze;

GLuint LoadShader(GLenum type,const char *file_name); // laduje program shadera z zewnetrznego pliku 

public:
	float Alpha;

	float y_angle;

	float rot_x; //  obrot obiektu wzgledem X
	float rot_y; //  obrot obiektu wzgledem Y
	float rot_z; //  obrot obiektu wzgledem Z

	float FogDensity;

	//z lab 3
	float xe;
	float ye;
	float yt;
	float ye_start = 2.0f;
	float ze;
	float xt;
	float zt;
	float R;
	float alfa;
	float height_beta;
	float d;
	float dx;
	float dy;
	float dz;
	//end

float LightAmbient;

glObject* ConeTorus;
glObject* Cube;
glObject* Suzanne;
glObject* Axes;
glObject* Plane;
glObject* Obj;
glObject* Projectile;
glObject* Sculpt;
glObject* House;
glObject* Panther;
glObject* Snowman;
glObject* Cybertruck;

glPrinter* Prn;

bool cone;

void PreparePrograms(); // przygotowuje programy przetwarzania 
void PrepareObjects(); // przygotowuje obiekty do wyswietlenia 

// rozmiary sceny 
int width;
int height;

float przesuniecie;

int frame;
glm::vec3 clickPos;
glm::vec3 clickDir;

glm::mat4 mProjection;
glm::mat4 mModelView;

// zasoby programu przetwarzania
GLuint program;
GLuint vertex_shader;
GLuint fragment_shader;

// komunikaty diagnostyczne 
char _msg[1024];
int err;
};

#endif
//------------------------------- KONIEC PLIKU -----------------------------------------------

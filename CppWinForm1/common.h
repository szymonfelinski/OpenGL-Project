//------------------------------- PROJECT SETTINGS --------------------------------------------
#ifndef _common_h
#define _common_h
#define IS_WIN32 1

#define PROJECT_NAME "lab8"
#define INITIAL_WIDTH 800
#define INITIAL_HEIGHT 600
#define PI 3.14159265
// disable warnings concerning deprecation of string operations (Visual Studio)
#define _CRT_SECURE_NO_WARNINGS
//------------------------------- DEPENDENCIES -----------------------------------------------
#pragma warning( disable : 4394 )
#include <windows.h>

#include "GL\glew.h"
#pragma comment(lib,"glew32.lib")	
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"opengl32.lib")	

#include <stdio.h>
#include <string.h>
#include <math.h>

//------------------------------- ROUTINES -----------------------------------------------
void PrintLog(char *message);
void ThrowException(System::String^ msg);
void Msg(char* msg);

#endif
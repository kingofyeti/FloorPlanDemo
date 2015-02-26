#include <AntTweakBar.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <gl/glut.h>
#include <opencv2\opencv.hpp>
#include <math.h>
#include "imageProcess.h"

struct Vec3
{
	float x;
	float y;
	float z;
};

struct Triangle
{
	Vec3 v0;
	Vec3 v1;
	Vec3 v2;
	Vec3 Norm[3];
	Vec3 face_normal;
	int Color[3];
};
typedef enum { SHAPE_TEAPOT = 1, SHAPE_TORUS, SHAPE_CONE } Shape;

const int MAX_MATERIAL_COUNT = 100000;

const double PI = 3.14159265;

Vec3 rotationVector = { -1, -1, -1 };
Vec3 camera = { 1000.0f, 1000.0f, 1000.0f };
Vec3 center = { 0.0f, 0.0f, 0.0f };
Vec3 ocamera = { 1000.0f, 1000.0f, 1000.0f };
Vec3 orotationVector = { -1, -1, -1 };
Vec3 up = { 0.0f, 0.0f, 1.0f };

float wallHeight = 100;


float width = 1366.0f;
float height = 768.0f;

Vec3 ambient[MAX_MATERIAL_COUNT], diffuse[MAX_MATERIAL_COUNT], specular[MAX_MATERIAL_COUNT];

float shine[MAX_MATERIAL_COUNT];

int material_count, NumTris, color_index[3], i;

Triangle *Tris;
float rtri;


char ch;

int zfar = 1000000;

int fileNum = 0;
int clickNum = 0;

bool frontFace = false;
bool clipPlane = false;

float LightAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float LightDiffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
float LightPosition[] = { 2000.0f, 2000.0f, 2000.0f, 1.0f };
GLuint	texture[3];

bool rotationBall = false;
bool movementBall = false;
bool zoomBall = false;
bool isDrag = false;
int autoRotate = 0;

// 0:points		1:lines		2:polygons
int modelState = 1;

int zoomRatio = 100;
int dragRatio = 200;

int clickMouseX;
int clickMouseY;
int curMouseX;
int curMouseY;
double mouseVectorX;
double mouseVectorY;

int preButton; // 1 left, 2 right, 3 mid


// Shapes scale
float g_Zoom = 1.0f;
// Shape orientation (stored as a quaternion)
float g_Rotation[] = { 0.0f, 0.0f, 0.0f, 1.0f };
// Auto rotate
int g_AutoRotate = 0;
int g_RotateTime = 0;
float g_RotateStart[] = { 0.0f, 0.0f, 0.0f, 1.0f };
// Shapes material
float g_MatAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float objectColor[] = { 0.2f, 0.2f, 0.2f };

float g_MatDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
// Light parameter
float g_LightMultiplier = 1.0f;
float g_LightDirection[] = { -0.57735f, -0.57735f, -0.57735f };

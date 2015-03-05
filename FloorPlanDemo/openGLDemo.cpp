//  ---------------------------------------------------------------------------
//
//  @file       FloorPlanDemo
//  @tools used: 
//              AntTweakBar: http://anttweakbar.sourceforge.net/doc
//              OpenGL:      http://www.opengl.org
//				OpenCV:		 http://www.opencv.org
//              GLUT:        http://opengl.org/resources/libraries/glut
//  
//  @author     Yeqi Wang
//  @date       02/25/2015
//
//  ---------------------------------------------------------------------------
#include "openGLDemo.h"

using namespace std;

void inputFile(char *FileName)
{
	FILE* fp;
	fopen_s(&fp, FileName, "r");
	if (fp == NULL) { printf("ERROR: unable to open TriObj [%s]!\n", FileName); exit(1); }
	fscanf_s(fp, "%c", &ch);
	while (ch != '\n') // skip the first line
		fscanf_s(fp, "%c", &ch);

	fscanf_s(fp, "# triangles = %d\n", &NumTris); // read # of triangles
	fscanf_s(fp, "Material count = %d\n", &material_count); // read material count


	for (i = 0; i < material_count; i++) {
		fscanf_s(fp, "ambient color %f %f %f\n", &(ambient[i].x), &(ambient[i].y), &(ambient[i].z));
		fscanf_s(fp, "diffuse color %f %f %f\n", &(diffuse[i].x), &(diffuse[i].y), &(diffuse[i].z));
		fscanf_s(fp, "specular color %f %f %f\n", &(specular[i].x), &(specular[i].y), &(specular[i].z));
		fscanf_s(fp, "material shine %f\n", &(shine[i]));
	}


	fscanf_s(fp, "%c", &ch);
	while (ch != '\n') // skip documentation line
		fscanf_s(fp, "%c", &ch);


	printf("Reading in %s (%d triangles). . .\n", FileName, NumTris);
	Tris = new Triangle[NumTris];

	for (i = 0; i < NumTris; i++) // read triangles
	{
		fscanf_s(fp, "v0 %f %f %f %f %f %f %d\n",
			&(Tris[i].v0.x), &(Tris[i].v0.y), &(Tris[i].v0.z),
			&(Tris[i].Norm[0].x), &(Tris[i].Norm[0].y), &(Tris[i].Norm[0].z),
			&(color_index[0]));
		fscanf_s(fp, "v1 %f %f %f %f %f %f %d\n",
			&(Tris[i].v1.x), &(Tris[i].v1.y), &(Tris[i].v1.z),
			&(Tris[i].Norm[1].x), &(Tris[i].Norm[1].y), &(Tris[i].Norm[1].z),
			&(color_index[1]));
		fscanf_s(fp, "v2 %f %f %f %f %f %f %d\n",
			&(Tris[i].v2.x), &(Tris[i].v2.y), &(Tris[i].v2.z),
			&(Tris[i].Norm[2].x), &(Tris[i].Norm[2].y), &(Tris[i].Norm[2].z),
			&(color_index[2]));
		fscanf_s(fp, "face normal %f %f %f\n", &(Tris[i].face_normal.x), &(Tris[i].face_normal.y),
			&(Tris[i].face_normal.z));

		Tris[i].Color[0] = (unsigned char)(int)(255 * (diffuse[color_index[0]].x));
		Tris[i].Color[1] = (unsigned char)(int)(255 * (diffuse[color_index[0]].y));
		Tris[i].Color[2] = (unsigned char)(int)(255 * (diffuse[color_index[0]].z));
	}
	fclose(fp);
}

void printVector3f(Vec3 vector){
	cout << "Vector: " << vector.x << " " << vector.y << " " << vector.z << endl;
}

void normalizeVector3D(Vec3 &vector)
{
	float magnitude = sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
	vector.x /= magnitude;
	vector.y /= magnitude;
	vector.z /= magnitude;
}

Vec3 calculateNewRotation(Vec3 rotationVector, float hAngle, float pAngle){
	normalizeVector3D(rotationVector);
	hAngle = hAngle * PI / 180.0;
	pAngle = pAngle * PI / 180.0;
	Vec3 temp = rotationVector;
	rotationVector.x = cos(hAngle)*temp.x - sin(hAngle)*temp.y;
	rotationVector.y = sin(hAngle)*temp.x + cos(hAngle)*temp.y;
	rotationVector.z = rotationVector.z;
	//temp = rotationVector;
	//Vec3 temp2 = { -temp.y, temp.x, 0 };
	//normalizeVector3D(temp2); 
	//Vec3 temp3 = rotationVector;
	//rotationVector.x = (cos(pAngle) + temp2.x*temp2.x*(1 - cos(pAngle))) * temp3.x
	//					+ temp2.x*temp2.y*(1 - cos(pAngle))*temp3.y
	//					+ temp2.y*sin(pAngle)*temp3.z;
	//rotationVector.y = temp2.y*temp2.x*(1 - cos(pAngle))*temp3.x
	//					+ (cos(pAngle) + temp2.y*temp2.y*(1 - cos(pAngle)))*temp3.y
	//					+ (-temp2.x*sin(pAngle))*temp3.z;
	//rotationVector.z = temp2.y*(-sin(pAngle))*temp3.x
	//					+ temp2.x*sin(pAngle)*temp3.y
	//					+ cos(pAngle);
	return rotationVector;
}

Vec3 calculateMoveCamera(Vec3 camera, Vec3 rotationVector, float x, float y){
	x /= 10;
	y /= 10;
	float tempx = rotationVector.x;
	float tempy = rotationVector.y;
	camera.x += x * (-tempy) * dragRatio;
	camera.y += x * tempx * dragRatio;
	camera.z += y * dragRatio;
	return camera;
}

Vec3 calculateZoomCamera(Vec3 camera, Vec3 rotationVector){
	Vec3 temp = rotationVector;
	camera.x += temp.x;
	camera.y += temp.y;
	camera.z += temp.z;
	return camera;
}

void inputData(int fileNum){
	switch (fileNum)
	{
	case 0:
		inputFile("models/cow_up.in");
		break;
	case 1:
		inputFile("models/phone.in");
		break;
	case 2:
		inputFile("models/cube.in");
		break;
	default:
		break;
	}
	cout << "Input finished" << endl;
}

void reshape(int w, int h)
{
	GLfloat aspect = (GLfloat)w / (GLfloat)h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, aspect, 0.1f, zfar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	TwWindowSize(w, h);
}

void drawAxisAndBase(){
	glPushMatrix();
	glBegin(GL_LINES);
	for (int i = -10000; i < 10000; i += 1000){
		glColor3f(0.72, 0.75, 1.0);
		glVertex3f(10000.0f, (GLfloat)i, 0);
		glVertex3f(-10000.0f, (GLfloat)i, 0);
	}
	for (int i = -10000; i < 10000; i += 1000){
		glColor3f(0.72, 0.75, 1.0);
		glVertex3f((GLfloat)i, 10000.0f, 0);
		glVertex3f((GLfloat)i, -10000.0f, 0);
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glBegin(GL_LINES);
	// draw line for x axis Red
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(10000.0, 0.0, 0.0);
	// draw line for y axis Green
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 10000.0, 0.0);
	// draw line for Z axis Blue
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 10000.0);
	glEnd();
	glPopMatrix();
	glPushMatrix();
	glRotatef(rtri, 0.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	glColor3f(1.0, 1.0, 0.43);
	glVertex3f(0.0, 0.0, 1000.0);
	glVertex3f(0.0, 0.0, 1020.0);
	glVertex3f(50.0, 0.0, 1000.0);
	glEnd();
	rtri += 1;
	glPopMatrix();
}

void setCamera(){
	if ((rotationBall || movementBall) && isDrag){
		mouseVectorX = (GLdouble)(curMouseX - clickMouseX) / 10;
		mouseVectorY = (GLdouble)(curMouseY - clickMouseY) / 10;
		if (rotationBall == true){
			gluLookAt(camera.x* g_Zoom, camera.y* g_Zoom, camera.z* g_Zoom,
				camera.x* g_Zoom + calculateNewRotation(rotationVector, mouseVectorX, mouseVectorY).x,
				camera.y* g_Zoom + calculateNewRotation(rotationVector, mouseVectorX, mouseVectorY).y,
				camera.z* g_Zoom + calculateNewRotation(rotationVector, mouseVectorX, mouseVectorY).z,
				up.x, up.y, up.z);
		}
		if (movementBall == true){
			//cout << mouseVectorX << " " << mouseVectorY << endl;
			gluLookAt(calculateMoveCamera(camera, rotationVector, mouseVectorX, mouseVectorY).x * g_Zoom,
				calculateMoveCamera(camera, rotationVector, mouseVectorX, mouseVectorY).y * g_Zoom,
				calculateMoveCamera(camera, rotationVector, mouseVectorX, mouseVectorY).z * g_Zoom,
				calculateMoveCamera(camera, rotationVector, mouseVectorX, mouseVectorY).x * g_Zoom + rotationVector.x,
				calculateMoveCamera(camera, rotationVector, mouseVectorX, mouseVectorY).y * g_Zoom + rotationVector.y,
				calculateMoveCamera(camera, rotationVector, mouseVectorX, mouseVectorY).z * g_Zoom + rotationVector.z,
				up.x, up.y, up.z);
		}
	}
	else{
		gluLookAt(camera.x * g_Zoom, camera.y* g_Zoom, camera.z* g_Zoom,
			camera.x * g_Zoom + rotationVector.x, camera.y* g_Zoom + rotationVector.y, camera.z* g_Zoom + rotationVector.z,
			up.x, up.y, up.z);
	}
}

void drawModel(){

	if (clickNum != fileNum){
		fileNum = clickNum;
		inputData(fileNum);
	}
	if (frontFace = true)
		glFrontFace(GL_CCW);
	else
		glFrontFace(GL_CW);

	glTranslatef(0, 0, 500);
	if (autoRotate == 1)
		glRotatef(-rtri, 0.0f, 1.0f, 0.0f);
	glRotatef(90, 0, 0, 1);
	glRotatef(90, 1, 0, 0);
	switch (modelState)
	{
	case 0:
		glPointSize(1.0f);
		glBegin(GL_POINTS);
		for (int i = 0; i < NumTris; i++){
			glColor3f(objectColor[0], objectColor[1], objectColor[2]);
			glVertex3f(Tris[i].v0.x, Tris[i].v0.y, Tris[i].v0.z);
			glVertex3f(Tris[i].v1.x, Tris[i].v1.y, Tris[i].v1.z);
			glVertex3f(Tris[i].v2.x, Tris[i].v2.y, Tris[i].v2.z);
		}
		glEnd();
		break;
	case 1:
		glBegin(GL_LINES);
		for (int i = 0; i < NumTris; i++){
			glColor3f(objectColor[0], objectColor[1], objectColor[2]);
			glVertex3f(Tris[i].v0.x, Tris[i].v0.y, Tris[i].v0.z);
			glVertex3f(Tris[i].v1.x, Tris[i].v1.y, Tris[i].v1.z);
			glVertex3f(Tris[i].v1.x, Tris[i].v1.y, Tris[i].v1.z);
			glVertex3f(Tris[i].v2.x, Tris[i].v2.y, Tris[i].v2.z);
			glVertex3f(Tris[i].v2.x, Tris[i].v2.y, Tris[i].v2.z);
			glVertex3f(Tris[i].v0.x, Tris[i].v0.y, Tris[i].v0.z);
		}
		glEnd();
		break;
	case 2:
		glFrontFace(GL_CCW);
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < NumTris; i++){
			glColor3f(objectColor[0], objectColor[1], objectColor[2]);
			//glColor3f(0.2, 0.2, 0.2);
			glNormal3f(Tris[i].face_normal.x, Tris[i].face_normal.y, Tris[i].face_normal.z);
			//glTexCoord2f(0.0f, 0.0f);
			glVertex3f(Tris[i].v0.x, Tris[i].v0.y, Tris[i].v0.z);
			//glTexCoord2f(1.0f, 0.0f);
			glVertex3f(Tris[i].v1.x, Tris[i].v1.y, Tris[i].v1.z);
			//glTexCoord2f(1.0f, 1.0f);
			glVertex3f(Tris[i].v2.x, Tris[i].v2.y, Tris[i].v2.z);
		}
		glEnd();
		break;
	default:
		break;
	}
	glPopMatrix();
}

void drawHouse(){

	switch (modelState)
	{
	case 0:
		glPointSize(5.0f);
		glBegin(GL_POINTS);
		for (int i = 0; i < contours_poly.size(); i++){
			for (int j = 0; j < contours_poly[i].size();j++){
				glColor3f(objectColor[0], objectColor[1], objectColor[2]);
				glVertex3f(contours_poly[i][j].x, contours_poly[i][j].y, 0);
				//glVertex3f(contours_poly[i][j].x, contours_poly[i][j].y, wallHeight);
			}
		}
		glEnd();
		break;
	case 1:
		for (int i = 0; i < contours_poly.size(); i++){
			//glBegin(GL_LINES);
			for (int j = 0; j < contours_poly[i].size()-1; j++){
				glBegin(GL_LINES);
				glColor3f(objectColor[0], objectColor[1], objectColor[2]);
				glVertex3f(contours_poly[i][j].x, contours_poly[i][j].y, 0);
				glVertex3f(contours_poly[i][j + 1].x, contours_poly[i][j + 1].y, 0);
				glVertex3f(contours_poly[i][j + 1].x, contours_poly[i][j + 1].y, 0);
				glVertex3f(contours_poly[i][j + 1].x, contours_poly[i][j + 1].y, wallHeight);
				glVertex3f(contours_poly[i][j + 1].x, contours_poly[i][j + 1].y, wallHeight);
				glVertex3f(contours_poly[i][j].x, contours_poly[i][j].y, wallHeight);
				glVertex3f(contours_poly[i][j].x, contours_poly[i][j].y, wallHeight);
				glVertex3f(contours_poly[i][j].x, contours_poly[i][j].y, 0);
				glEnd();
			}
			//glEnd();
		}
		break;
	case 2:
		for (int i = 0; i < contours_poly.size(); i++){
			for (int j = 0; j < contours_poly[i].size() - 1; j++){
				glColor3f(objectColor[0], objectColor[1], objectColor[2]);
				glBegin(GL_POLYGON);
				glVertex3f(contours_poly[i][j].x, contours_poly[i][j].y, 0);
				glVertex3f(contours_poly[i][j + 1].x, contours_poly[i][j + 1].y, 0);
				glVertex3f(contours_poly[i][j + 1].x, contours_poly[i][j + 1].y, wallHeight);
				glVertex3f(contours_poly[i][j].x, contours_poly[i][j].y, wallHeight);
				glVertex3f(contours_poly[i][j].x, contours_poly[i][j].y, 0);
				glEnd();
			}
			
		}
		break;
		break;
	default:
		break;
	}


}

void render(){

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glLightModeli(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);	// Important
	glEnable(GL_NORMALIZE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (clipPlane)
		glEnable(GL_CLIP_PLANE1);
	else
		glDisable(GL_CLIP_PLANE1);
	double equation[] = { -1.0f, -1.0f, 1.0f, 1.0f };
	glClipPlane(GL_CLIP_PLANE1, equation);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_LIGHTING);								// Important!
	glLightfv(GL_LIGHT1, GL_AMBIENT, g_MatAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, g_MatDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, g_LightDirection);
	glEnable(GL_LIGHT1);

	glEnable(GL_COLOR_MATERIAL);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, g_MatAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, g_MatDiffuse);

	glLoadIdentity();
	setCamera();
	drawAxisAndBase();
	//drawModel();
	drawHouse();
	TwDraw();
	glutPostRedisplay();
	glutSwapBuffers();
}

void printCameraInfo(){
	cout << "Camera: " << camera.x << " " << camera.y << " " << camera.z << endl;
	cout << "Center: " << center.x << " " << center.y << " " << center.z << endl;
	cout << "Up: " << up.x << " " << up.y << " " << up.z << endl;
}

void keyFunc(unsigned char key, int x, int y){

	if (!TwEventKeyboardGLUT(key, x, y)){
		switch (key)
		{
		case 'r':
			break;
		case '.':
			modelState += 1;
			if (modelState == 3) modelState = 0;
			break;
		default:
			break;
		}
	}

	//printCameraInfo();
}

void drag(int x, int y){
	if (!TwEventMouseMotionGLUT(x, y))
	{
		isDrag = true;
		curMouseX = x;
		curMouseY = y;
	}
}

void mouseFunc(int button, int state, int x, int y){

	if (!TwEventMouseButtonGLUT(button, state, x, y)){
		if (state == GLUT_DOWN){
			if (button == GLUT_LEFT_BUTTON){
				preButton = 1;
				movementBall = true;
			}
			if (button == GLUT_RIGHT_BUTTON){
				preButton = 2;
				rotationBall = true;
			}
		}
		else{
			isDrag = false;
			movementBall = false;
			rotationBall = false;
			if (preButton == 1){
				camera = calculateMoveCamera(camera, rotationVector, mouseVectorX, mouseVectorY);
			}
			if (preButton == 2){
				rotationVector = calculateNewRotation(rotationVector, mouseVectorX, mouseVectorY);
			}

		}
		clickMouseX = x;
		clickMouseY = y;
	}

}

void TW_CALL  onResetButtonClicked(void *clientData)
{
	camera = ocamera;
	g_Zoom = 1.0;
	rotationVector = orotationVector;
}

void TW_CALL onChangeModelButtonClicked(void *clientData){
	modelState++;
	if (modelState == 3) modelState = 0;
}

void TW_CALL onAutoRotateButtonClicked(void *clientData){
	autoRotate++;
	if (autoRotate == 2) autoRotate = 0;
}

void TW_CALL onFrontFaceButtonClicked(void *clientData){
	frontFace = !frontFace;
}

void TW_CALL onClipPlaneButtionClicked(void *clientData){
	clipPlane = !clipPlane;
}

void TwBarFunc(){
	TwBar *bar;
	TwInit(TW_OPENGL, NULL);
	TwGLUTModifiersFunc(glutGetModifiers);
	bar = TwNewBar("Control Bar");
	TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLUT and OpenGL.' ");
	TwDefine(" TweakBar size='200 800' color='96 216 224' ");
	TwAddVarRW(bar, "Zoom", TW_TYPE_FLOAT, &g_Zoom,
		" min=0.01 max=3.0 step=0.01 keyIncr=z keyDecr=Z help='Scale the object (1=original size).' ");

	//TwAddVarRW(bar, "Zfar", TW_TYPE_FLOAT, &zfar,
	//	" min=100 max=100000 step=1000 help='Scale the Zfar (10000=original size).' ");

	TwAddButton(bar, "Reset", onResetButtonClicked, NULL, " label='Reset Location: ' ");
	TwAddButton(bar, "Front Face: ", onFrontFaceButtonClicked, NULL, " label='Font face change: ' ");
	TwAddButton(bar, "Clip Plane: ", onClipPlaneButtionClicked, NULL, " label='Clip plane change: ' ");
	TwAddButton(bar, "ChangeModel", onChangeModelButtonClicked, NULL, "label = 'Change Model Type: '");
	TwAddButton(bar, "AutoRotate", onAutoRotateButtonClicked, NULL, "label = 'Auto Rotate: '");
	TwAddVarRW(bar, "Model index: ", TW_TYPE_INT8, &clickNum, "min=0 max=2 step=1 label='Model index:");
	TwAddVarRW(bar, "LightDir", TW_TYPE_DIR3F, &g_LightDirection,
		" label='Light direction' opened=true help='Change the light direction.' ");
	TwAddVarRW(bar, "Object", TW_TYPE_COLOR3F, &objectColor, " group='objectColor' ");
	TwAddVarRW(bar, "Ambient", TW_TYPE_COLOR3F, &g_MatAmbient, " group='Light' ");
	TwAddVarRW(bar, "Diffuse", TW_TYPE_COLOR3F, &g_MatDiffuse, " group='Light' ");
}

int main(int argc, char* argv[]){
	openCVProcess(1);
	glutInit(&argc, argv);
	cout << "hello GL" << endl;
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Assignment 1");

	inputData(fileNum);
	glutKeyboardFunc(keyFunc);
	glutMotionFunc(drag);
	glutMouseFunc(mouseFunc);
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);

	glutDisplayFunc(render);
	glutReshapeFunc(reshape);

	
	TwBarFunc();
	glutMainLoop();//enters the GLUT event processing loop.

	return 0;
}


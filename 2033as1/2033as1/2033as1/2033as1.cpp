// glutwindow.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include "glut.h"
#include "imageloader.h"

using namespace std;

void display();
void reshape(int w, int h);
void idle();
void mouse_click(int button, int state, int x, int y);
void mouse_motion(int x, int y);
void cal_vertices();
void read_vertices();
void init();
void GetOGLPos(int x, int y, double &posX, double &posY, double &posZ);
void planet(GLfloat, GLuint&);
void loadTexture(const char* filename, GLuint &ID);

// global var, rotation angle
float angle = 0.0;
//moon rotation
float angle2 = 0.0;
float vertices[5][2]; // 5 vertices, and x, y coordinates
GLuint listname = 1;
float dx = 0.0;
float dy = 0.0; 
float dz = 0.0;
float scale = 1.0;

// mouse click
int x_click, y_click;
int button;

GLuint EarthID, MoonID;

//planet radiuses 
GLfloat EarthRadius = 0.8;
GLfloat MoonRadius = 0.4;

int main(int argc, char* argv[]) {
	// initialize glut
	glutInit(&argc, argv);

	// initialize window position
	glutInitWindowPosition(10, 10);
	
	// window size
	glutInitWindowSize(1200, 600);

	// display mode
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	// create a window
	glutCreateWindow("A simple glut window");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMouseFunc(mouse_click);
	glutMotionFunc(mouse_motion);

	init();

	// glut main loop
	glutMainLoop();

	return 0;
}

void display(){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	glPushMatrix();
		//scales the planet system based of the user input
		glScalef(scale, scale, scale);
		//translates the planet system based on the user input
		glTranslatef(dx, dy, dz);
		//rotates the planet system based on the user input
		glRotatef(angle, 1.0, 1.0, 1.0);
		
		//drawing earth throught the planet function
		planet(EarthRadius, EarthID);

		glPushMatrix(); {
			glRotatef(angle2, 0.0, 1.0, 0.0);
			glTranslatef(1.5, 1.0, 0.0);
			//drawing moon through the planet function which rotates around earth
			planet(MoonRadius, MoonID);

		}

		glPopMatrix();
	glPopMatrix();
	
	glutSwapBuffers();
}

void reshape(int w, int h) {
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	float ratio = 1.0* w / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45,ratio,1,1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0,0.0,5.0,     // eye's position
			  0.0,0.0,1.0,     // center of interest
			  0.0f,1.0f,0.0f); // up direction
}

void idle() {
	//angle = angle + 0.1;
	angle2 = angle2 + 0.1;
	glutPostRedisplay();
}

void mouse_click(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		x_click = x;
		y_click = y;
		::button = button; // button = GLUT_LEFT_BUTTON, GLUT_RIGHT_BUTTON, GLUT_MIDDLE_BUTTON
	}else {
		::button = -1;
	}
}

void mouse_motion(int x, int y) {
	double mvmatrix[16];
	double projmatrix[16];
	int    viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

	//mouse position calculations 
	double cur_y = double(viewport[3] - y);
	double obj_cur_y, obj_cur_x, obj_cur_z; 
	gluUnProject((double)x, cur_y, 0.5, mvmatrix, projmatrix, viewport, &obj_cur_x, &obj_cur_y, &obj_cur_z);

	double pre_y = double(viewport[3] - y_click);
	double obj_pre_y, obj_pre_x, obj_pre_z; 
	gluUnProject((double)x_click, pre_y, 0.5, mvmatrix, projmatrix, viewport, &obj_pre_x, &obj_pre_y, &obj_pre_z);


	//switch case function for determening to which mouse button is pressed
	switch (button){
		case GLUT_LEFT_BUTTON:
			dx += (obj_cur_x - obj_pre_x) * 2.5;
			dy -= (obj_pre_y - obj_cur_y) * 2.5;
			break;
		case GLUT_RIGHT_BUTTON:
			angle -=(x_click - x);
			break;
		case GLUT_MIDDLE_BUTTON:
			scale -= (x_click - x) * 0.02;
			break;
	}
	x_click = x;
	y_click = y;
}

void cal_vertices() {
	// open a file for writing pentagen vertices 
	ofstream outfile("vertices.txt");
	float a = 2 * 3.1415926 / 5.0;
	float x1 = 0.0, y1 = 1.0;
	outfile << x1 << " " << y1 << endl;;
	float x, y;
	for (int i = 1; i < 5; i++) {
		x = x1 * cos(a * i) - y1 * sin(a * i);
		y = y1 * cos(a * i) + x1 * sin(a * i);
		outfile << x << " " << y << endl;;
	}
	outfile.close();
}

void read_vertices() {
	ifstream infile("vertices.txt");
	for (int i = 0; i < 5; i++){
		infile >> vertices[i][0];
		infile >> vertices[i][1];
		cout << vertices[i][0] << " " << vertices[i][1] << endl;
	}
}

void init() {
	glEnable(GL_DEPTH);
	cal_vertices();
	read_vertices();

	glNewList(listname, GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	for(int i = 0; i < 5; i++){
		glVertex3f(vertices[i][0], vertices[i][1], 0.0);
	}
	glEnd();
	glEndList();

	//loading the textures for the planets thought the loadTexture function
	loadTexture("earth.bmp", EarthID);
	loadTexture("moon.bmp", MoonID);

	//how the image should behave when loaded onto the sphere
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

}

void GetOGLPos(int x, int y, double &posX, double &posY, double &posZ) {
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
 
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev(GL_PROJECTION_MATRIX, projection );
	glGetIntegerv(GL_VIEWPORT, viewport );
 
	winX = (float)x;
	winY = (float)viewport[3] - (float)y - 1;
	// glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
 
	gluUnProject( winX, winY, 0.5, modelview, projection, viewport, &posX, &posY, &posZ);
}

void loadTexture(const char* filename, GLuint &ID)
{

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	std::cout << ID << std::endl;

	//the function gets the data from the file, applies the id to the variables
	Image* image = loadBMP(filename);
	//glTexImage2D(GL_TEXTURE_2D,
	//	0,
	//	GL_RGB,
	//	image->width,
	//	image->height,
	//	0,
	//	GL_RGB,
	//	GL_UNSIGNED_BYTE,
	//	image->pixels);

	gluBuild2DMipmaps(GL_TEXTURE_2D,
		3,
		image->width,
		image->height,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		image->pixels);


	delete image;
}

void planet(GLfloat radius, GLuint &textureID)
{
	//binds the texture to the created planet
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	//glDisable(GL_TEXTURE_2D);

	
	float slices = 132;
	float stacks = 132;

	GLUquadricObj* QuadObj = gluNewQuadric();
	gluQuadricOrientation(QuadObj, GLU_OUTSIDE);
	gluQuadricDrawStyle(QuadObj, GLU_FILL);
	gluQuadricNormals(QuadObj, GLU_SMOOTH);
	gluQuadricTexture(QuadObj, GLU_TRUE);

	gluSphere(QuadObj, radius, slices, stacks);

	glDisable(GL_TEXTURE_2D);
}
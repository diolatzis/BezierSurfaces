#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL\glut.h>
#include <vector>
//	Ken Shoemake's ArcBall	
#include "BallAux.h"
#include "Ball.h"

//	Vector 3d class
#include "Vec3.h"

#ifndef PI
#define PI 3.14159265358979
#endif

static const int S_POINTS = 30;
static const int T_POINTS = 10;

typedef Vec3<int> Triangle3;

static GLfloat controlPoints[4][4][3] =
{ // controlPoints
	-3.0,  3.0,  0.01,
	-1.0,  3.0,  0.01,
	1.0,  3.0,  0.01,
	3.0,  3.0,  0.01,
	-3.0,  1.0,  4.01,
	-1.0,  1.0,  4.01,
	1.0,  1.0,  4.01,
	3.0,  1.0,  4.01,
	-3.0, -1.0, -4.01,
	-1.0, -1.0, -4.01,
	1.0, -1.0, -4.01,
	3.0, -1.0, -4.01,
	-3.0, -3.0,  0.01,
	-1.0, -3.0,  0.01,
	1.0, -3.0,  0.01,
	3.0, -3.0,  0.01,
}; // controlPoints

   // width and height of window, plus initial value
int windowSize = 640;
int windowWidth, windowHeight;

// an arcball for rotation
BallData ball;

// which vertex is being actively manipulated
int activeVertex = 0;

// whether to show the reference planes
int showPlanes = 1;
// whether to show the control net
int showNet = 1;
// whether to show the control vertices
int showVertices = 1;
// whether to show the surface
int showBezier = 0;
// whether to show the sphere
int showSphere = 0;
// whether to use glutSpheres
int useGLUTSphere = 0;
// whether to use wireframe for glutSphere
int useWireframe = 1;
int showBezPoints = 0;
int showBezWireFrame = 0;
int showBezFlat = 0;

// initialize the interface
void initInterface();

// draw routine
void display();
// reshape routine
void reshape(int width, int height);
// mouse up / down routine
void mouse(int button, int state, int x, int y);
// mouse drag routine
void mouseDrag(int x, int y);
// key down 
void keyDown(unsigned char key, int x, int y);
// special key down 
void specialDown(int key, int x, int y);
// help text
void printHelp();

// lighting information
GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat position[] = { 0.0, 0.3, 1.0, 0.0 };
GLfloat mat_diffuse[] = { 0.6, 0.6, 0.6, 1.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess[] = { 50.0 };

// initialize GLUT
void initInterface()
{ // initInterface()
  // set the window height and width
	windowWidth = windowSize;
	windowHeight = windowSize;
	// set the initial glut parameters
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("Bezier Patch");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseDrag);
	glutKeyboardFunc(keyDown);
	glutSpecialFunc(specialDown);
	glEnable(GL_DEPTH_TEST);

	// set the projection matrix to the identity
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-5.0, 5.0, -5.0, 5.0, -5.0, 5.0);
	// set the modelview matrix to the identity
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// set up lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	// rotate slightly to make grid visible
	glRotatef(7.0, 1.0, 1.0, 0.0);

	// initialize the arcball
	Ball_Init(&ball);
	Ball_Place(&ball, qOne, 0.75);


	printHelp();
} //	end of initInterface() 

  // mouse down routine
void mouse(int button, int state, int x, int y)
{ // mouse()
  // left button rotates
	if (button == GLUT_LEFT_BUTTON)
	{ // left button
		if (state == GLUT_DOWN)
		{ // left button down
		  // convert the control input into the vector type used
		  // by the arcball
			HVect vNow;
			vNow.x = (2.0 * x - glutGet(GLUT_WINDOW_WIDTH)) / glutGet(GLUT_WINDOW_WIDTH);
			vNow.y = -(2.0 * y - glutGet(GLUT_WINDOW_HEIGHT)) / glutGet(GLUT_WINDOW_HEIGHT);
			// update the arcball
			Ball_Mouse(&ball, vNow);
			Ball_BeginDrag(&ball);
		} // left button down
		else if (state == GLUT_UP)
		{ // left button up
			Ball_EndDrag(&ball);
		} // left button up
	} // left button
	  // either way, force a redisplay
	glutPostRedisplay();
} // mouse() 

  // mouse drag routine
void mouseDrag(int x, int y)
{ // mouseDrag()
  // convert the control input into the vector type used
  // by the arcball
	HVect vNow;
	vNow.x = (2.0 * x - glutGet(GLUT_WINDOW_WIDTH)) / glutGet(GLUT_WINDOW_WIDTH);
	vNow.y = -(2.0 * y - glutGet(GLUT_WINDOW_HEIGHT)) / glutGet(GLUT_WINDOW_HEIGHT);
	// update the arcball
	Ball_Mouse(&ball, vNow);

	// and force a refresh to highlight it
	glutPostRedisplay();
} // mouseDrag() 

  // window reshape routine
void reshape(int width, int height)
{ // reshape()
  // set the projection matrix to a simple orthogonal projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-5.0, 5.0, -5.0 * height / width, 5.0 * height / width, -5.0, 5.0);
	glViewport(0, 0, width, height);
} // reshape() 

  // key down (for quit key)
void keyDown(unsigned char key, int x, int y)
{ // keyDown()
	switch (key)
	{ // switch on key
	case '1':
		showBezPoints = !showBezPoints;
		break;
	case '2':
		showBezWireFrame = !showBezWireFrame;
		break;
	case '3':
		showBezFlat = !showBezFlat;
		break;
	case '<':
		activeVertex = (activeVertex + 15) % 16;
		break;
	case '>':
		activeVertex = (activeVertex + 1) % 16;
		break;
	case '+':
		controlPoints[activeVertex / 4][activeVertex % 4][2] += 0.1;
		break;
	case '-':
		controlPoints[activeVertex / 4][activeVertex % 4][2] -= 0.1;
		break;
	case 'p':
		showPlanes = !showPlanes;
		break;
	case 'v':
		showVertices = !showVertices;
		break;
	case 'n':
		showNet = !showNet;
		break;
	case 'b':
		showBezier = !showBezier;
		break;
	case 's':
		showSphere = !showSphere;
		break;
	case 'g':
		useGLUTSphere = !useGLUTSphere;
		break;
	case 'w':
		useWireframe = !useWireframe;
		break;
		// check for quit key
	case 'q': case 'Q':
		exit(0);
		break;
	} // switch on key
	  // force a redisplay
	glutPostRedisplay();
} // keyDown() 

  // special key down 
void specialDown(int key, int x, int y)
{ // specialDown()
	switch (key)
	{ // switch on key
	case GLUT_KEY_RIGHT:
		controlPoints[activeVertex / 4][activeVertex % 4][0] += 0.1;
		break;
	case GLUT_KEY_LEFT:
		controlPoints[activeVertex / 4][activeVertex % 4][0] -= 0.1;
		break;
	case GLUT_KEY_UP:
		controlPoints[activeVertex / 4][activeVertex % 4][1] += 0.1;
		break;
	case GLUT_KEY_DOWN:
		controlPoints[activeVertex / 4][activeVertex % 4][1] -= 0.1;
		break;
	} // switch on key
	  // force a redisplay
	glutPostRedisplay();
} // specialDown()

  // display routine
void display()
{ // display()
	int i;
	// set the clear colour and clear the frame buffer
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the modelview
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// arc ball rotation specified next
	GLfloat mNow[16];
	Ball_Update(&ball);
	Ball_Value(&ball, mNow);
	glMultMatrixf(mNow);

	glDisable(GL_LIGHTING);

	if (showPlanes)
	{ // showPlanes
	  // set the drawing colour
		glColor3f(0.25, 0.0, 0.25);

		// draw a ground plane
		glBegin(GL_LINES);
		for (i = -5; i <= 5; i += 2)
		{
			glVertex3f(i, 0, -5);
			glVertex3f(i, 0, 5);
			glVertex3f(-5, 0, i);
			glVertex3f(5, 0, i);
		}
		glEnd();

		glColor3f(0.0, 0.25, 0.25);
		glBegin(GL_LINES);
		for (i = -5; i <= 5; i += 2)
		{
			glVertex3f(0, i, -5);
			glVertex3f(0, i, 5);
			glVertex3f(0, -5, i);
			glVertex3f(0, 5, i);
		}
		glEnd();

		glColor3f(0.25, 0.25, 0.0);
		glBegin(GL_LINES);
		for (i = -5; i <= 5; i++)
		{
			glVertex3f(-5, i, 0);
			glVertex3f(5, i, 0);
			glVertex3f(i, -5, 0);
			glVertex3f(i, 5, 0);
		}
		glEnd();
	} // showPlanes

	  // now render a sphere
	glColor3f(0.3, 0.3, 1.25);
	if (showSphere)
		if (useGLUTSphere)
			if (useWireframe)
				glutWireSphere(3.0, 120, 120);
			else
			{ // solid
				glEnable(GL_LIGHTING);
				glutSolidSphere(3.0, 120, 120);
				glDisable(GL_LIGHTING);
			} // solid
		else
		{ // points
			glBegin(GL_POINTS);
			for (float phi = 0.0; phi < PI / 2.0; phi += PI / 1000.0)
				for (float theta = 0.0; theta < PI / 2.0; theta += PI / 1000.0)
					glVertex3f(3.0 * cos(phi) * cos(theta), 3.0 * cos(phi) * sin(theta), 3.0 * sin(phi));
			glEnd();
		} // points
		  // now render the control vertices
	if (showVertices)
	{ //  showVertices
		glMatrixMode(GL_MODELVIEW);
		// for each control vertex
		for (int id = 0; id < 16; id++)
		{ // for id	
			if (id == activeVertex)
				glColor3f(1.0, 0.0, 0.0);
			else
				glColor3f(0.75, 0.75, 0.75);
			glPushMatrix();
			glTranslatef(controlPoints[id / 4][id % 4][0],
				controlPoints[id / 4][id % 4][1],
				controlPoints[id / 4][id % 4][2]);
			glutSolidSphere(0.125, 12, 12);
			glPopMatrix();
		} // for id
	} // showVertices

	if (showNet)
	{ // showNet
		//Render the control net
		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_LINES);
		for (i = 0; i < 4; i ++)
		{
			//Connect the control points horizontally

			glVertex3f(controlPoints[i][0][0], controlPoints[i][0][1], controlPoints[i][0][2]);
			glVertex3f(controlPoints[i][1][0], controlPoints[i][1][1], controlPoints[i][1][2]);

			glVertex3f(controlPoints[i][1][0], controlPoints[i][1][1], controlPoints[i][1][2]);
			glVertex3f(controlPoints[i][2][0], controlPoints[i][2][1], controlPoints[i][2][2]);

			glVertex3f(controlPoints[i][2][0], controlPoints[i][2][1], controlPoints[i][2][2]);
			glVertex3f(controlPoints[i][3][0], controlPoints[i][3][1], controlPoints[i][3][2]);

			//Connect the control points vertically

			glVertex3f(controlPoints[0][i][0], controlPoints[0][i][1], controlPoints[0][i][2]);
			glVertex3f(controlPoints[1][i][0], controlPoints[1][i][1], controlPoints[1][i][2]);

			glVertex3f(controlPoints[1][i][0], controlPoints[1][i][1], controlPoints[1][i][2]);
			glVertex3f(controlPoints[2][i][0], controlPoints[2][i][1], controlPoints[2][i][2]);

			glVertex3f(controlPoints[2][i][0], controlPoints[2][i][1], controlPoints[2][i][2]);
			glVertex3f(controlPoints[3][i][0], controlPoints[3][i][1], controlPoints[3][i][2]);
		}
		glEnd();
	} // showNet

	//--------Compute Bezier surface--------------

	Vec3<float> bezConstructionPoints[4][4][4];
	Vec3<float> bezPoints[T_POINTS+1][S_POINTS+1];
	Vec3<float> bezNormals[T_POINTS+1][S_POINTS+1];

	//Initialize construction points
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			//Level 3 construction points
			bezConstructionPoints[i][j][3] = Vec3<float>(controlPoints[i][j][0], controlPoints[i][j][1], controlPoints[i][j][2]);
		}
	}



	float t = 0.0f;

	//Iterate through t
	for (int i = 0; i <= T_POINTS ; i++)
	{
		
		float s = 0.0f;

		//Iterate through s
		for (int j = 0 ; j <= S_POINTS ; j++)
		{

			//Compute levels until level 0 to get bezier point
			for (int level = 2; level >= 0; level--)
			{
				for (int u = 0; u <= level; u++)
				{
					for (int v = 0; v <= level; v++)
					{
						bezConstructionPoints[u][v][level] = bezConstructionPoints[u][v][level + 1] * ((1 - s)*(1 - t)) + bezConstructionPoints[u][v + 1][level + 1] * ((1 - s)*t) + bezConstructionPoints[u + 1][v][level + 1] * (s*(1 - t)) + bezConstructionPoints[u + 1][v + 1][level + 1] * (s*t);
					}
				}
			}

			//Add point to bezier array
			bezPoints[i][j] = bezConstructionPoints[0][0][0];

			//Compute difdis from the construction points
			Vec3<float> difdis = (bezConstructionPoints[0][0][3] * pow((1 - t), 3) + bezConstructionPoints[0][1][3] * 3 * t*pow((1 - t), 2)
				+ bezConstructionPoints[0][2][3] * 3 * pow(t, 2)*(1 - t) + bezConstructionPoints[0][3][3] * pow(t, 3))*(-3+6*s-3*pow(s,2)) +
				(bezConstructionPoints[1][0][3] * pow((1 - t), 3) + bezConstructionPoints[1][1][3] * 3 * t*pow((1 - t), 2)
					+ bezConstructionPoints[1][2][3] * 3 * pow(t, 2)*(1 - t) + bezConstructionPoints[1][3][3] * pow(t, 3)) *(3-12*s+9*pow(s,2)) +
					(bezConstructionPoints[2][0][3] * pow((1 - t), 3) + bezConstructionPoints[2][1][3] * 3 * t*pow((1 - t), 2)
						+ bezConstructionPoints[2][2][3] * 3 * pow(t, 2)*(1 - t) + bezConstructionPoints[2][3][3] * pow(t, 3)) * (6*s-9*pow(s,2)) +
						(bezConstructionPoints[3][0][3] * pow((1 - t), 3) + bezConstructionPoints[3][1][3] * 3 * t*pow((1 - t), 2)
							+ bezConstructionPoints[3][2][3] * 3 * pow(t, 2)*(1 - t) + bezConstructionPoints[3][3][3] * pow(t, 3))*3*pow(s,2);

			//Compute difdit from the construction points
			Vec3<float> difdit = (bezConstructionPoints[0][0][3] * pow((1 - s), 3) + bezConstructionPoints[1][0][3] * 3 * s*pow((1 - s), 2)
				+ bezConstructionPoints[2][0][3] * 3 * pow(s, 2)*(1 - s) + bezConstructionPoints[3][0][3] * pow(s, 3))*(-3 + 6 * t - 3 * pow(t, 2)) +
				(bezConstructionPoints[0][1][3] * pow((1 - s), 3) + bezConstructionPoints[1][1][3] * 3 * s*pow((1 - s), 2)
					+ bezConstructionPoints[2][1][3] * 3 * pow(s, 2)*(1 - s) + bezConstructionPoints[3][1][3] * pow(s, 3)) *(3 - 12 * t + 9 * pow(t, 2)) +
					(bezConstructionPoints[0][2][3] * pow((1 - s), 3) + bezConstructionPoints[1][2][3] * 3 * s*pow((1 - s), 2)
						+ bezConstructionPoints[2][2][3] * 3 * pow(s, 2)*(1 - s) + bezConstructionPoints[3][2][3] * pow(s, 3)) * (6 * t - 9 * pow(t,2)) +
						(bezConstructionPoints[0][3][3] * pow((1 - s), 3) + bezConstructionPoints[1][3][3] * 3 * s*pow((1 - s), 2)
							+ bezConstructionPoints[2][3][3] * 3 * pow(s, 2)*(1 - s) + bezConstructionPoints[3][3][3] * pow(s, 3)) * 3 * pow(t, 2);

			//Store the vertex normal
			bezNormals[i][j] = difdis.crossProduct(difdit).getNormal();

			//Step s
			s += (1 / (float)S_POINTS);
		}

		//Step t
		t += (1 / (float)T_POINTS);
	}

	//Tesselate the bezier surface using triangles
	std::vector<Triangle3> bezTriangles;

	for (int i = 0; i < T_POINTS; i++)
	{
		for (int j = 0; j < S_POINTS; j++)
		{
			//Break each quad to 2 triangles
			bezTriangles.push_back(Triangle3(i*(S_POINTS + 1) +j,(i+1)*(S_POINTS + 1) +j,i*(S_POINTS + 1) +j+1));			//Save the index of the vertices 
			bezTriangles.push_back(Triangle3((i+1)*(S_POINTS + 1) +j,(i+1)*(S_POINTS + 1) +j+1,i*(S_POINTS + 1) +j+1));		//in row-major format
		}
	}
	
	//Show the Bezier surface points
	if (showBezPoints)
	{
		for (int i = 0; i <= T_POINTS; i++)
		{
			for (int j = 0; j <= S_POINTS; j++)
			{
				glBegin(GL_POINTS);
				glColor3f(1, 1, 1);
				glVertex3f(bezPoints[i][j].getX(), bezPoints[i][j].getY(), bezPoints[i][j].getZ());
				glEnd();
			}
		}

	}

	//Show the wireframe of the triangles
	if (showBezWireFrame)
	{
		for (int i = 0; i < bezTriangles.size(); i++)
		{
			//Retrieve vertices from row-major format
			Vec3<float> v0, v1, v2;
			v0 = bezPoints[bezTriangles.at(i)[0] / (S_POINTS+1)][bezTriangles.at(i)[0] % (S_POINTS + 1)];
			v1 = bezPoints[bezTriangles.at(i)[1] / (S_POINTS + 1)][bezTriangles.at(i)[1] % (S_POINTS + 1)];
			v2 = bezPoints[bezTriangles.at(i)[2] / (S_POINTS + 1)][bezTriangles.at(i)[2] % (S_POINTS + 1)];


			//3. Wireframe tesselation
			glBegin(GL_LINE_STRIP);
			glColor3f(1, 1, 1);
			glVertex3f(v0.getX(), v0.getY(), v0.getZ());
			glVertex3f(v1.getX(), v1.getY(), v1.getZ());
			glVertex3f(v2.getX(), v2.getY(), v2.getZ());
			glVertex3f(v0.getX(), v0.getY(), v0.getZ());
			glEnd();
		}
	}

	//Show the Bezier surface using flat triangle format
	if (showBezFlat)
	{ // showBezFlat

		for (int i = 0; i < bezTriangles.size(); i++)
		{
			//Retrieve vertices from row-major format
			Vec3<float> v0, v1, v2, n;
			v0 = bezPoints[bezTriangles.at(i)[0] / (S_POINTS + 1)][bezTriangles.at(i)[0] % (S_POINTS + 1)];
			v1 = bezPoints[bezTriangles.at(i)[1] / (S_POINTS + 1)][bezTriangles.at(i)[1] % (S_POINTS + 1)];
			v2 = bezPoints[bezTriangles.at(i)[2] / (S_POINTS + 1)][bezTriangles.at(i)[2] % (S_POINTS + 1)];
			//Compute flat normal from triangle vertices
			n = (v2 - v0).crossProduct(v1 - v0).getNormal();

			glEnable(GL_LIGHTING);
			glNormal3f(n.getX(),n.getY(),n.getZ());
			glBegin(GL_TRIANGLES);
	
			glColor3f(1, 1, 1);
			glVertex3f(v0.getX(), v0.getY(), v0.getZ());
			glVertex3f(v1.getX(), v1.getY(), v1.getZ());
			glVertex3f(v2.getX(), v2.getY(), v2.getZ());
			glEnd();
		}
		
		
	} // showBezFlat

	//Show the Bezier surface with the correct vertex normals
	if (showBezier)
	{
		for (int i = 0; i < bezTriangles.size(); i++)
		{

			//Retrieve vertices from row-major format
			Vec3<float> v0, v1, v2, n0, n1, n2;
			v0 = bezPoints[bezTriangles.at(i)[0] / (S_POINTS + 1)][bezTriangles.at(i)[0] % (S_POINTS + 1)];
			v1 = bezPoints[bezTriangles.at(i)[1] / (S_POINTS + 1)][bezTriangles.at(i)[1] % (S_POINTS + 1)];
			v2 = bezPoints[bezTriangles.at(i)[2] / (S_POINTS + 1)][bezTriangles.at(i)[2] % (S_POINTS + 1)];

			//Retrieve vertices normals from row-major format
			n0 = bezNormals[bezTriangles.at(i)[0] / (S_POINTS + 1)][bezTriangles.at(i)[0] % (S_POINTS + 1)];
			n1 = bezNormals[bezTriangles.at(i)[1] / (S_POINTS + 1)][bezTriangles.at(i)[1] % (S_POINTS + 1)];
			n2 = bezNormals[bezTriangles.at(i)[2] / (S_POINTS + 1)][bezTriangles.at(i)[2] % (S_POINTS + 1)];

			glEnable(GL_LIGHTING);

			glBegin(GL_TRIANGLES);
			glColor3f(1, 1, 1);
			glNormal3f(n0.getX(), n0.getY(), n0.getZ());
			glVertex3f(v0.getX(), v0.getY(), v0.getZ());
			glNormal3f(n1.getX(), n1.getY(), n1.getZ());
			glVertex3f(v1.getX(), v1.getY(), v1.getZ());
			glNormal3f(n2.getX(), n2.getY(), n2.getZ());
			glVertex3f(v2.getX(), v2.getY(), v2.getZ());
			glEnd();
		}
	}

	 // make sure we pop our matrix
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// swap the buffers
	glutSwapBuffers();
	} // display() 

	//	main()
	int main(int argc, char **argv)
	{ // main()
	  // initialize GLUT
		glutInit(&argc, argv);
		// set initial values
		initInterface();
		// start the event loop
		glutMainLoop();
		// and return when done
		return 0;
	} //	end of main() 

	void printHelp()
	{ // printHelp()
		printf("<     Previous control point\n");
		printf(">     Next control point\n");
		printf("Left  Decrement control point's x coordinate\n");
		printf("Right Increment control point's x coordinate\n");
		printf("Up    Increment control point's y coordinate\n");
		printf("Down  Decrement control point's y coordinate\n");
		printf("+     Increment control point's z coordinate\n");
		printf("-     Decrement control point's z coordinate\n");
		printf("p     Toggle reference planes\n");
		printf("v     Toggle control vertices\n");
		printf("n     Toggle control net\n");
		printf("b     Toggle bezier patch\n");
		printf("s     Toggle sphere\n");
		printf("q     Toggle sphere\n");
		printf("1     Toggle Bezier points\n");
		printf("2     Toggle Bezier wireframe\n");
		printf("3     Toggle Bezier with flat normals\n");
	} // printHelp()

	
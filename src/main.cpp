//****************************************************
// Author: Jun-Yan Zhu
// Email: junyanz@berkeley.edu
// UC Berkeley
// 3rd party softwares:
// -- OpenCV
// -- OpenGL and glut
// -- CLM Tracker by Jason Saragih
//****************************************************

#include "common.h"
#include "FaceDemo.h"
#include <opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class Viewport {
public:
    int w; // width
    int h; // height
};



// Global Variables
Viewport g_viewport;
CFaceDemo* g_demo;
InputMode g_inputMode;
string g_fileName;

// transformation 
float g_transX = 0.0f; 
float g_transY = 0.0f; 
float g_scale = 1.0f; 
float g_rotateY = 0.0f; 
float g_rotateX = 0.0f;
float g_eye = 5.0f; 


float g_eyex = 0.5;
double modelview[16]; 
GLdouble k1 = 0.0;
GLdouble k2 = 0.0;
GLfloat shear_factor = 0.0;
GLfloat s =0.0f;
GLfloat k = 0.0;
GLfloat xl = 0.0;
GLfloat xr = 0.0;

//GLfloat(*pM) [4] = M;

// rendering flag
bool g_smooth = true; 
bool g_filled = true; 
bool g_transform = false;
//bool g_perspective = false; 
bool g_perspective = true; 


void movedistance(){
    xl = 0.1 * k / 5.0f;
    xr = 0.1 * k / 5.0f;
}

void displayMatrix(GLfloat (&m)[4*4]){
  std::cout<<endl;
  for(int j=0;j<4;++j){
    for(int i=0;i<4;++i)
      std::cout<<m[i,j]<<ends;
      std::cout<<endl;
    }
}

void dispalyVector(GLfloat (&m)[16]){
  std::cout<<endl;
  for(int i = 0; i<15;i++)
    std::cout<<m[i]<<ends;
  std::cout<<endl;
}

void perspectiveGL( GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar )
{
    const GLdouble pi = 3.1415926535897932384626433832795;
    GLdouble fW, fH;

    fH = tan( fovY / 360 * pi ) * zNear;
    fW = fH * aspect;
    //std::cout<<fW<<endl;
    movedistance();
    glFrustum( -fW + xl , fW + xr , -fH, fH, zNear, zFar );
}

// OpenGL functions
void myReshape(int w, int h) { 
    g_viewport.w = w;
    g_viewport.h = h;

    glViewport(0,0,g_viewport.w,g_viewport.h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    double near_z = 0.1;
    double far_z = 500.0;
    double fovy = 2 * atan(0.5/g_eye) * 180 / PI;
    double aspect = 1.0;

    if (g_perspective) {
        //gluPerspective(fovy, aspect, near_z, far_z);  
	perspectiveGL(fovy, aspect, near_z, far_z);
        //gluLookAt(0.5,0.5,g_eye, 0.5,0.5,0, 0,1,0);
	gluLookAt(g_eyex,0.5 ,g_eye, 0.5,0.5,0, 0,1,0);
    } else {
        glOrtho(0, 1, 0, 1, near_z, far_z);   
        gluLookAt(0,0,100, 0,0,0, 0,1,0);
    }
}

void initScene(){
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glMatrixMode(GL_MODELVIEW);

    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    myReshape(g_viewport.w,g_viewport.h);
}

void keyPressed (unsigned char key, int x, int y) {  
    switch (key) {
    case '=':
        if (g_perspective) {
            g_eye *= 1.1;
            myReshape(g_viewport.w,g_viewport.h);
        } else
            g_scale *= 1.1;
        break;
    case '-':
        if (g_perspective) {
            g_eye /= 1.1;
            myReshape(g_viewport.w,g_viewport.h);
        } else
            g_scale /= 1.1;
        break;
    case 's':
        g_smooth = !g_smooth;
        if (g_smooth)
            DEBUG_INFO("smooth shading");
        else
            DEBUG_INFO("flat shading");
        break;
    case 'w':
        g_filled = !g_filled;
        if (g_filled)
            DEBUG_INFO("filled mode");
        else
            DEBUG_INFO("wireframe mode");
        break;
    case 'q':
        DEBUG_INFO("quit the program");
        exit(0);
        break;
    case 't':
        g_transform = !g_transform;
        if (g_transform)
            DEBUG_INFO("transformation mode");
        else
            DEBUG_INFO("static mode");
        break;
    case 'p':
        g_perspective = !g_perspective;
        if (g_perspective)
            DEBUG_INFO("perspective mode");
        else
            DEBUG_INFO("orthogonal mode");
        myReshape(g_viewport.w,g_viewport.h);
        break;
    case 'r':
      if(g_perspective){
// 	g_eyex += 0.2;
//  	k1 -= 0.0001;
// 	k2 -= 0.0002;
	//std::cout<<k<<endl;
	//s -= 0.005;
	//s *= 1.1;
	//std::cout<<s<<endl;
	k -= 0.01;
	myReshape(g_viewport.w, g_viewport.h);
      }
      break;
    default:
        DEBUG_INFO("invalid keyboard input");
        break;
    }
} 

void arrowKeyPressed(int key, int x, int y) {
    int mod = glutGetModifiers();
    switch (key) {
    case GLUT_KEY_UP:
        if (mod == GLUT_ACTIVE_SHIFT)
            g_transY += 0.01f;
        else
            g_rotateY -= 5.0f;
        break;
    case GLUT_KEY_DOWN:
        if (mod == GLUT_ACTIVE_SHIFT)
            g_transY -= 0.01f;
        else
            g_rotateY += 5.0f;
        break;
    case GLUT_KEY_LEFT:
        if (mod == GLUT_ACTIVE_SHIFT)
            g_transX -= 0.01f;
        else
            g_rotateX -= 5.0f;
        break;
    case GLUT_KEY_RIGHT:
        if (mod == GLUT_ACTIVE_SHIFT)
            g_transX += 0.01f;
        else
            g_rotateX += 5.0f;
        break;
    default:
        break;
    }

}

//display functions
void myDisplay() {
  GLfloat shearMatric[16] = {1.0f,0.0,0.0f,0.0f,
			  s,1.0f,0.0f,0.0f,
			  0.0f,0.0f,1.0f,0.0f,
			  0.0f,0.0f,0.0f,1.0f}; 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPushMatrix();
    
    glTranslatef(g_transX, g_transY, 0.0f);
    glRotatef(g_rotateY, 1.0f, 0.0f, 0.0f);
    glRotatef(g_rotateX, 0.0f, 1.0f, 0.0f);
    glScalef(g_scale, g_scale, g_scale);
//     glLoadMatrixf(shearMatric);
//     glMultMatrixf(shearMatric);
    //std::cout<<shearMatric<<endl;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    //std::cout<<modelview<<endl;
    Mat texture = g_demo->Texture();
    if (texture.empty())
        return;
    Mat Z = g_demo->Depth();
    int width = Z.cols;
    int height = Z.rows;
    float size = (float)max(width, height);
//    float offset = (height-width)/float(2*height);
    Mat ar, H;
    Point2d t;
    g_demo->Transform(ar, H, t);

    if (g_transform) {
        double m[16] = {0};
        FOR (i, 3) FOR (j, 3) m[4*i+j] = ar.at<double>(j, i);
        m[15]=1.0f;

        glMultMatrixd(m);                      // 3d warp
        glTranslated(t.x/size, t.y/size, 0);  //translation
    }


    if (g_smooth)
        glShadeModel(GL_SMOOTH);
    else
        glShadeModel(GL_FLAT);

    if (g_filled)
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    else
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    // draw 3d face
    int dx[] = {0, 0, 1, 1};
    int dy[] = {0, 1, 1, 0};

    FOR (w, width-1) {
        FOR (h, height-1) {
            bool flag = true;
            FOR (k, 4)
                    flag = flag && (Z.at<double>(h+dy[k], w+dx[k]) > 0.0f);

            if (flag) {
                glBegin(GL_QUADS);
                FOR (k, 4) {
                    int hh = h  + dy[k];
                    int ww = w  + dx[k];
		    
                    float r = texture.at<Vec3b>(hh,ww)[2]/255.0f;
                    float g = texture.at<Vec3b>(hh,ww)[1]/255.0f;
                    float b = texture.at<Vec3b>(hh,ww)[0]/255.0f;
                    glColor3f(r,g,b);
                    float x = ww/size;
                    float y = (height-hh)/size; //(height-hh)/size;
                    float z = (float)Z.at<double>(hh,ww)/size;
		    //std::cout<<x<<" "<<y<<" "<<z<<" "<<endl;
                    glVertex3f(x,y,z);
                }
                glEnd();
            }
        }
    }

    // get opencv image
    unsigned char* buffer = new unsigned char[(int)(size*size*3)];
    glReadPixels(0, 0, (int)size, (int)size, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    cv::Mat img((int)size, (int)size, CV_8UC3, buffer);
    cvtColor(img, img, CV_RGB2BGR);
    flip(img, img, 0);
    Mat face = Mat();
    face = img(Rect(0, 0, width, height));

    // 2d warp
    invertAffineTransform(H, H);
    Size imgSize = g_demo->ImageSize();
    Mat warpImg = Mat::zeros(imgSize, CV_8UC3);
    warpAffine(face, warpImg, H, imgSize);

    imshow("Projected Face", warpImg);
    waitKey(1);
    glPopMatrix();
    glutSwapBuffers();
}



void myFrameMove() {
    //nothing here for now
#ifdef _WIN32
    Sleep(10);
#endif
    glutPostRedisplay();
}


void PrintHelp() {
    printf("./FaceDemo mode filePath\n");
    printf("Example (image): ./FaceDemo image ./data/bush.jpg\n");
    printf("Example (video): ./FaceDemo video ./data/eli.avi\n");
}

void ParseCommand(int _argc, char* _argv[]) {
    if (_argc != 3)
        PrintHelp();

    string mode = string(_argv[1]);

    if (mode == "image")
        g_inputMode = IM_IMAGE;
    else if (mode == "video")
        g_inputMode = IM_VIDEO;
    else {
        DEBUG_ERROR("INPUT_MODE = invalid");
        PrintHelp();
    }

    g_fileName = string(_argv[2]);
}


int main(int argc, char *argv[]) {
    glutInit(&argc, argv);

    ParseCommand(argc, argv);
    g_demo = new CFaceDemo(g_inputMode, g_fileName);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    g_viewport.w = 224;
    g_viewport.h = 224;
    //g_viewport.w = 348;
    //g_viewport.h = 348;
    
    glutInitWindowSize(g_viewport.w, g_viewport.h);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("3D Face");

    initScene();
    glutKeyboardFunc(keyPressed);
    glutSpecialFunc(arrowKeyPressed);
    glutDisplayFunc(myDisplay);
    glutReshapeFunc(myReshape);
    glutIdleFunc(myFrameMove);
    glutMainLoop();
    return 0;
}

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
#include<string.h>
#include<iostream>
# include <stdio.h>
# include <stdlib.h>
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
int dx=0; 
char ad[128]={0};
int movenum = 50;
int i = 1;
int whilenum = -1000;

// rendering flag
bool g_smooth = true; 
bool g_filled = true; 
bool g_transform = false;
//bool g_perspective = false; 
bool g_perspective = true;
void myReshape(int w, int h);
void myDisplay();

void PrintMatrix( float matrix[16] )  
{  
    assert( matrix != 0 );  
    printf( "%8.2f%8.2f%8.2f%8.2f\n"  
            "%8.2f%8.2f%8.2f%8.2f\n"  
            "%8.2f%8.2f%8.2f%8.2f\n"  
            "%8.2f%8.2f%8.2f%8.2f\n",  
            matrix[0], matrix[1], matrix[2], matrix[3],  
            matrix[4], matrix[5], matrix[6], matrix[7],  
            matrix[8], matrix[9], matrix[10], matrix[11],  
            matrix[12], matrix[13], matrix[14], matrix[15] );  
}  

void MyRotatef( float matrix[16],  
                float angleInDegree,  
                float x,  
                float y,  
                float z )  
{  
    assert( matrix != 0 );  
  
    // 向量的单位化  
    float length = sqrt( x * x + y * y + z * z );  
    assert( !qFuzzyCompare( length, 0.0f ) );// 希望length不为0  
  
    x /= length;  
    y /= length;  
    z /= length;  
  
    float alpha = angleInDegree / 180 * 3.1415926;// 已转换弧度制  
    float s = sin( alpha );  
    float c = cos( alpha );  
    float t = 1.0f - c;  
  
#define MATRIX( row, col ) matrix[row * 4 + col]  
    MATRIX( 0, 0 ) = t * x * x + c;  
    MATRIX( 0, 1 ) = t * x * y + s * z;  
    MATRIX( 0, 2 ) = t * x * z - s * y;  
    MATRIX( 0, 3 ) = 0.0f;  
    MATRIX( 1, 0 ) = t * x * y - s * z;  
    MATRIX( 1, 1 ) = t * y * y + c;  
    MATRIX( 1, 2 ) = t * y * z + s * x;  
    MATRIX( 1, 3 ) = 0.0f;  
    MATRIX( 2, 0 ) = t * x * z + s * y;  
    MATRIX( 2, 1 ) = t * y * z - s * x;  
    MATRIX( 2, 2 ) = t * z * z + c;  
    MATRIX( 2, 3 ) = 0.0f;  
    MATRIX( 3, 0 ) = 0.0f;  
    MATRIX( 3, 1 ) = 0.0f;  
    MATRIX( 3, 2 ) = 0.0f;  
    MATRIX( 3, 3 ) = 1.0f;  
#undef MATRIX  
}  
  

void batchprocess(){
   //1、视景体移动;2、把目标移到窗口;3、save 2D image; 4、循环50次
  std::cout<<"开始移动"<<endl;
  g_transX += 0.1f;
//   for(int i = 0;i<num; i++){
//     std::cout<<"移动第"<<i<<"次"<<endl;
//     k -= 0.05;
//     myReshape(g_viewport.w, g_viewport.h);
//   }
  
  /*
	g_transX	k
left 	-0.088f		-0.09
right 	+0.0958f	+0.1
   
   */
  
  while(movenum){
    std::cout<<"移动第"<<i<<"次"<<endl;
    k -= 0.09;
    myReshape(g_viewport.w, g_viewport.h);
    g_transX -= 0.088f;
    myDisplay();
    i++;
    movenum--;
    
  }
}

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
    fW = fH * aspect;//0.1
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
//         gluLookAt(0.0,0.5,-5.0, 0.5,0.5,0, 0,1,0);
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
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
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
	k -= 0.1;
	myReshape(g_viewport.w, g_viewport.h);
      }
      break;
    case 'l':
      if(g_perspective){
	k += 0.1;
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
            g_transX -= 0.1f;
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
  float matrix1[16], matrix2[16];
  GLfloat shearMatric[16] = {1.0f,0.0,0.0f,0.0f,
			  s,1.0f,0.0f,0.0f,
			  0.0f,0.0f,1.0f,0.0f,
			  0.0f,0.0f,0.0f,1.0f}; 

//    GLfloat rotateMatric[16] = { cos(rotatex / 180 * 3.1415926),0.0f,sin(rotatex / 180 * 3.1415926),0.0f,
// 				0.0f, 1.0f,0.0f,0.0f,
// 				-sin(rotatex / 180 * 3.1415926),0.0f,cos(rotatex / 180 * 3.1415926),0.0f,
// 				0.0f,0.0f,0.0f,1.0f};
			  glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
//     glDepthFunc(GL_LEQUAL);
    glDepthFunc(GL_LESS);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();    
    glTranslatef(g_transX, g_transY, 0.0f);
    glRotatef(g_rotateY, 1.0f, 0.0f, 0.0f);
    glRotatef(g_rotateX, 0.0f, 1.0f, 0.0f);
    glScalef(g_scale, g_scale, g_scale);
//       PrintMatrix(rotateMatric);
//     glLoadMatrixf(rotateMatric);
//     glMultMatrixf(rotateMatric);
  
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    Mat texture = g_demo->Texture(); //画出原图的面部特征
  
    if (texture.empty())
        return;
    Mat Z = g_demo->Depth();
    int width = Z.cols ;//174
    int height = Z.rows;//224

    float size = (float)max(width, height) ;//224.0
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

    // draw 3d face (opengl)
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
//                     int ww = (width - w)  + dx[k];
		    int ww = w + dx[k];
		    
                    float r = texture.at<Vec3b>(hh,ww)[2]/255.0f;
                    float g = texture.at<Vec3b>(hh,ww)[1]/255.0f;
                    float b = texture.at<Vec3b>(hh,ww)[0]/255.0f;
                    glColor3f(r,g,b);
                    float x = ww/size;
                    float y = (height-hh)/size; //(height-hh)/size;
                    float z = (float)Z.at<double>(hh,ww)/size;
//  		    while(whilenum>0){
// 		      std::cout<<x<<","<<y<<","<<z<<endl;
// 		      whilenum = -1000;
//  		    }
                    glVertex3f(x,y,z);
// 		    whilenum++;
                }
                glEnd();
            }
        }
    }

    // get opencv image
    unsigned char* buffer = new unsigned char[(int)(size*size*3)];
    glReadPixels(0, 0, (int)size, (int)size, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    cv::Mat img((int)size , (int)size , CV_8UC3, buffer);
    cvtColor(img, img, CV_RGB2BGR);
    flip(img, img, 0);
    Mat face = Mat();
    face = img(Rect(0, 0, (int)size, (int)size));

    // 2d warp not use
//     invertAffineTransform(H, H);
//     Size imgSize = g_demo->ImageSize();
//     //Size bigimgsize = Size(1280,720);
//     Mat warpImg = Mat::zeros(imgSize, CV_8UC3);
//     Size bigH = Size(1024, 768);
//     warpAffine(face, warpImg, H, imgSize);
    
      imshow("Projected Face", face);
//       imwrite("screen/test.jpg", face);

    
       string imgfilename("img");
       char filenameNum[4];
//        int j = i;
//        if(j%2 != 0){j++;j=j/2;}
//        else j = j/2;
//        j = 27 - j;
//        sprintf(filenameNum, "%d", j);
//        imgfilename += filenameNum;
//        imgfilename += ".jpg"; 
//        std::cout<<imgfilename<<endl;
//        std::cout<<"存入第"<<j<<"张图"<<endl;
//        imwrite(imgfilename, face);
       
       int j = i;
       if (j%2 !=0 ){j++;j=j/2;}
       else j = j / 2;
       j = j + 25;
       sprintf(filenameNum, "%d", j);
       imgfilename += filenameNum;
       imgfilename += ".jpg";
       std::cout<<imgfilename<<endl;
       std::cout<<"存入第"<<j<<"张图"<<endl;
       imwrite(imgfilename, face);
       
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
    
    glutInitWindowSize(g_viewport.w, g_viewport.h);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("3D Face");

    initScene();
    glutKeyboardFunc(keyPressed);
    glutSpecialFunc(arrowKeyPressed);
    glutDisplayFunc(myDisplay);
    glutReshapeFunc(myReshape);
    glutIdleFunc(myFrameMove);
     batchprocess();
    glutMainLoop();
    return 0;
}

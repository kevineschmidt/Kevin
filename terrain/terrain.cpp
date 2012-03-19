/**********************************************************************
Copyright (C) 2011 by OpenEye Scientific Software, Inc.
***********************************************************************/
#include <QApplication>
#include <QtOpenGL>
#include <glut.h>
#include <math.h>

const unsigned int FIELD_SIZE = 513;
const float        H_FACT     = .85f;
float              HEIGHT_FIELD[FIELD_SIZE*FIELD_SIZE];
const unsigned int CLOUD_SIZE=257;
char cloudtex[CLOUD_SIZE*CLOUD_SIZE*4];


void BuildGrid(float *, unsigned int sz);

void BuildClouds()
{
  float CLOUDS_FIELD[129*129];
  CLOUDS_FIELD[00] =
    CLOUDS_FIELD[128]=
    CLOUDS_FIELD[128*129] =
    CLOUDS_FIELD[129*129-1] = 0;

  BuildGrid(CLOUDS_FIELD, 129);
  for (unsigned int i = 0; i < 129*129; ++i)
  {
    float val = CLOUDS_FIELD[i];
    if (val > 1)
      val = 1;
    if (val < -1)
      val = -1;
    float sc = (1.0f - val)/2;
    cloudtex[4*i  ] =  sc*255;
    cloudtex[4*i+1] =  sc*255;
    cloudtex[4*i+2] =  sc*35 + 220;
    cloudtex[4*i+3] =  255;
  }
         
}
template <class T, class U, class V>
void OEGeom3DCrossProd(T *x, const U *p, const V *q)
{
  x[0] = (T)(p[1]*q[2] - p[2]*q[1]);
  x[1] = (T)(p[2]*q[0] - p[0]*q[2]);
  x[2] = (T)(p[0]*q[1] - p[1]*q[0]);
}
template<class T,class U>
void OEGeom3DRotate(T *xyz, const U *m, unsigned int ncoord)
{
  double a=(double)m[0],b=(double)m[1],c=(double)m[2];
  double d=(double)m[3],e=(double)m[4],f=(double)m[5];
  double g=(double)m[6],h=(double)m[7],i=(double)m[8];

  for (unsigned int j = 0; j < ncoord; ++j)
  {
    double x = (double)xyz[j*3]; 
    double y = (double)xyz[j*3+1]; 
    double z = (double)xyz[j*3+2];
    xyz[j*3]   = (T)(a*x + b*y + c*z);
    xyz[j*3+1] = (T)(d*x + e*y + f*z);
    xyz[j*3+2] = (T)(g*x + h*y + i*z);
  }
}

void SetColor(float h)
{
  if (h <= 3)
    glColor3ub(0, 120, 0); // Green
  else if (h > 5)
    glColor3ub(255,255, 255); // White
  else if( h < 4)
  {
    float frac = h-3.0f;
    glColor3ub(130*frac, 120 + -40*frac, 0);
  }
  else if (h < 5)
  {
    float frac = h-4.0f;
    glColor3ub(130 + (255-130)*frac, 80 + (255-80)*frac, 255*frac);
  }
}

class MyGL : public QGLWidget
{
public:
  MyGL() : QGLWidget()
  {
    _x = 0;
    _z = 0;

    forward[0] = forward[1] = 0; forward[2] = 1.0f;
    left[0] = 1.0f; left[1] = left[2] = 0.0f;
  }
  float _z;
  float _x;
  float forward[3];
  float left[3];
  
  void initializeGL()
  {
    GLfloat light_ambient[]  = {.5f, .5f, .5f, 1.f};
    GLfloat light_specular[] = {0.f, 0.f, 0.f, 1.f};
    GLfloat light_diffuse[]  = {.8f, 0.8f, 0.8f, 1.f};
    GLfloat light_position[] = {1.f, .5f, 1.f, 0.f}; /* w=0 ==> directional */

    glLightfv (GL_LIGHT1, GL_POSITION, light_position);
    glLightfv (GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv (GL_LIGHT1, GL_SPECULAR, light_specular);

    glLightModelfv (GL_LIGHT_MODEL_AMBIENT, light_ambient);
    glEnable (GL_LIGHT1);
    glEnable(GL_LIGHTING);
    
    glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable (GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_FOG);
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_CULL_FACE);

    glShadeModel(GL_SMOOTH);
  }
  
  void keyPressEvent(QKeyEvent *k)
  {
    if (k->key() == Qt::Key_R)
    {
      HEIGHT_FIELD[0] =
        HEIGHT_FIELD[FIELD_SIZE-1] =
        HEIGHT_FIELD[(FIELD_SIZE-1)*FIELD_SIZE] =
        HEIGHT_FIELD[FIELD_SIZE*FIELD_SIZE-1] = 3;
      
      BuildGrid((float *)HEIGHT_FIELD, FIELD_SIZE);
      BuildClouds();
      updateGL();
    }
    if (k->key() == Qt::Key_Up)
    {
      _z+=forward[2];
      _x+=forward[0];
      updateGL();
    }
    else if (k->key() == Qt::Key_Down)
    {
      _z-=forward[2];
      _x-=forward[0];
      updateGL();
    }
    else if (k->key() == Qt::Key_Left)
    {
      float angle = -5.0f*3.1415/180.0f;
      float M[9] = {cos(angle), 0, -sin(angle),
                    0, 1, 0,
                    sin(angle), 0, cos(angle)};
      OEGeom3DRotate(forward, M, 1);
      OEGeom3DRotate(left, M, 1);
      
      updateGL();
    } 
    else if (k->key() == Qt::Key_Right)
    {
      float angle = 5.0f*3.1415/180.0f;
      float M[9] = {cos(angle), 0, -sin(angle),
                    0, 1, 0,
                    sin(angle), 0, cos(angle)};
      OEGeom3DRotate(forward, M, 1);
      OEGeom3DRotate(left, M, 1);
      updateGL();
    }
    else if (k->key() == Qt::Key_F)
    {
      if (glIsEnabled(GL_FOG))
        glDisable(GL_FOG);
      else
        glEnable(GL_FOG);
    }
  }


  float GetHeight(float x, float z)
  {
    int xIdx1 = _x;
    int yIdx1 = _z;
    int xIdx2 = _x+1;
    int yIdx2 = _z+1;

    while (xIdx1 < 0)
      xIdx1 += FIELD_SIZE-1;
    while (xIdx1 >= FIELD_SIZE)
      xIdx1 -= FIELD_SIZE-1;
    while (xIdx2 < 0)
      xIdx2 += FIELD_SIZE-1;
    while (xIdx2 >= FIELD_SIZE)
      xIdx2 -= FIELD_SIZE-1;

    while (yIdx1 < 0)
      yIdx1 += FIELD_SIZE-1;
    while (yIdx1 >= FIELD_SIZE)
      yIdx1 -= FIELD_SIZE-1;
    while (yIdx2 < 0)
      yIdx2 += FIELD_SIZE-1;
    while (yIdx2 >= FIELD_SIZE)
      yIdx2 -= FIELD_SIZE-1;
        

    return (HEIGHT_FIELD[(int)xIdx1 + FIELD_SIZE*(int)yIdx1] +
            HEIGHT_FIELD[(int)xIdx2 + FIELD_SIZE*(int)yIdx2])/2.0f;
  }

  void CalcNormal(int x, int y)
  {
    int xplus1  = x+1;
    int xminus1 = x-1;
    int yplus1  = y+1;
    int yminus1 = y-1;
    if (xplus1 >= FIELD_SIZE)
      xplus1 -= FIELD_SIZE - 1;
    if (xminus1 < 0)
      xminus1 += FIELD_SIZE-1;
    if (yplus1 >= FIELD_SIZE)
      yplus1 -= FIELD_SIZE - 1;
    if (yminus1 < 0)
      yminus1 += FIELD_SIZE-1;
    
    float normal1[3], normal2[3], normal3[3],normal4[3], normal[3];
    float v1[] = {0, HEIGHT_FIELD[x+ FIELD_SIZE*yplus1]  - HEIGHT_FIELD[x+ FIELD_SIZE*y], 1};
    float v2[] = {1, HEIGHT_FIELD[xplus1+ FIELD_SIZE*y]  - HEIGHT_FIELD[x+ FIELD_SIZE*y], 0};
    float v3[] = {0, HEIGHT_FIELD[x+ FIELD_SIZE*yminus1] - HEIGHT_FIELD[x+ FIELD_SIZE*y], -1};
    float v4[] = {-1, HEIGHT_FIELD[xminus1+ FIELD_SIZE*y] - HEIGHT_FIELD[x+ FIELD_SIZE*y], 0};
    
    OEGeom3DCrossProd(normal1, v1, v2);
    OEGeom3DCrossProd(normal2, v2, v3);
    OEGeom3DCrossProd(normal3, v3, v4);
    OEGeom3DCrossProd(normal4, v4, v1);

    normal[0] = (normal1[0] + normal2[0] + normal3[0] + normal4[0])/4.0f;
    normal[1] = (normal1[1] + normal2[1] + normal3[1] + normal4[1])/4.0f;
    normal[2] = (normal1[2] + normal2[2] + normal3[2] + normal4[2])/4.0f;
    glNormal3f(normal[0], normal[1], normal[2]);
  }
  
  void paintGL()
  {
    glViewport(0, 0, width(), height());
    glClearColor(.5, .5, 1.0, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, width()/(float)height(),.1, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float h = GetHeight(_x, _z);
    gluLookAt(_x, h+ 2, _z,
              _x + 100*forward[0], h+ 2.2, 100*forward[2] + _z,
              0, 1, 0);


    float fog[4] = {1.0f, 1.0f, 1.0f, 0};
    RenderClouds();

    glFogfv(GL_FOG_COLOR, fog);
    glHint(GL_FOG_HINT, GL_FASTEST);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 75);
    glFogf(GL_FOG_END,  150);


    glBegin(GL_TRIANGLES);

    int sizeXHalf = 200;
    int lowi  = -sizeXHalf + sizeXHalf*forward[0];
    int highi =  sizeXHalf + sizeXHalf*forward[0];
    int lowj  = -sizeXHalf + sizeXHalf*left[0];
    int highj =  sizeXHalf + sizeXHalf*left[0]; 

    for (int i = lowi; i < highi; ++i)
    {
      for (int j = lowj; j < highj; ++j)
      {
        float x = (int)(i + _x);
        float y = (int)(j + _z);
        
        int xIdx1 = x+.5f;
        int xIdx2 = (x+1)+.5f;
        int yIdx1 = (y)+.5f;
        int yIdx2 = (y+1)+.5f;

        while (xIdx1 < 0)
          xIdx1 += FIELD_SIZE-1;
        while (xIdx1 >= FIELD_SIZE)
          xIdx1 -= FIELD_SIZE-1;
        while (xIdx2 < 0)
          xIdx2 += FIELD_SIZE-1;
        while (xIdx2 >= FIELD_SIZE)
          xIdx2 -= FIELD_SIZE-1;

        while (yIdx1 < 0)
          yIdx1 += FIELD_SIZE-1;
        while (yIdx1 >= FIELD_SIZE)
          yIdx1 -= FIELD_SIZE-1;
        while (yIdx2 < 0)
          yIdx2 += FIELD_SIZE-1;
        while (yIdx2 >= FIELD_SIZE)
          yIdx2 -= FIELD_SIZE-1;

        CalcNormal(xIdx2, yIdx2);
        SetColor( HEIGHT_FIELD[xIdx2+ FIELD_SIZE*yIdx2]);
        glVertex3f(x+1, HEIGHT_FIELD[xIdx2+ FIELD_SIZE*yIdx2], y+1);

        CalcNormal(xIdx2, yIdx1);
        SetColor( HEIGHT_FIELD[xIdx2+ FIELD_SIZE*yIdx1]);
        glVertex3f(x+1, HEIGHT_FIELD[xIdx2+ FIELD_SIZE*yIdx1], y);

        CalcNormal(xIdx1, yIdx1);
        SetColor( HEIGHT_FIELD[xIdx1+ FIELD_SIZE*yIdx1]);
        glVertex3f(x,   HEIGHT_FIELD[xIdx1+ FIELD_SIZE*yIdx1], y);

        CalcNormal(xIdx1, yIdx2);
        SetColor( HEIGHT_FIELD[xIdx1+ FIELD_SIZE*yIdx2]);
        glVertex3f(x,   HEIGHT_FIELD[xIdx1+ FIELD_SIZE*yIdx2], y+1);

        CalcNormal(xIdx2, yIdx2);
        SetColor( HEIGHT_FIELD[xIdx2+ FIELD_SIZE*yIdx2]);
        glVertex3f(x+1, HEIGHT_FIELD[xIdx2+ FIELD_SIZE*yIdx2], y+1);

        CalcNormal(xIdx1, yIdx1);
        SetColor( HEIGHT_FIELD[xIdx1+ FIELD_SIZE*yIdx1]);
        glVertex3f(x, HEIGHT_FIELD[xIdx1+ FIELD_SIZE*yIdx1], y);
      }
    }

    glEnd();
        
    


    if (glGetError())
      printf ("GLERROR\n");
  }
  void RenderClouds()
  {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable (GL_TEXTURE_2D);
    GLuint m_texname;
    glGenTextures(1, &m_texname);
    glBindTexture(GL_TEXTURE_2D, m_texname);
//    glDisable (GL_DEPTH_TEST);
//    glDisable (GL_FOG);
//    glDisable (GL_LIGHTING);
//    glDisable (GL_BLEND);


		glTexImage2D (GL_TEXTURE_2D, 0, /* lod */
                  4, /* num components */
                  129, 129, /* width, height */
                  0, /* border */
                  GL_RGBA, GL_UNSIGNED_BYTE, /* format, type */
                  cloudtex);
    
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glClear (GL_COLOR_BUFFER_BIT);
    glTranslatef(_x , 0, _z);
    glColor3f(1, 1, 1);
    glDisable(GL_CULL_FACE);
    float scale = 100;
    glScalef(scale,scale,scale);
    glutSolidSphere(1, 30, 30);
    glScalef(1.0/scale, 1.0/scale, 1.0/scale);
    glTranslatef(-_x , 0, -_z);
    glPopAttrib ();
    glDeleteTextures(1, &m_texname);
  }
};

void DoSquare(float *field,int x, int y, int stride, float scale, unsigned int size)
{
  float v1 = field[x-stride + (y-stride)*size];
  float v2 = field[x+stride + (y+stride)*size];
  float v3 = field[x-stride + (y+stride)*size];
  float v4 = field[x+stride + (y-stride)*size];

  field[x + y*size] = (v1 + v2 + v4 + v3)/4.0f + scale * (.5f -  (float)rand()/(float)RAND_MAX);
}

float DoDiamond(float* field, int x, int y, int stride, float scale, int size )
{
  int x1, y1; // left
  int x2, y2; // top
  int x3, y3; // bottom
  int x4, y4; // right

  if (x==0)
    x1 = size-1 - stride;
  else
    x1 = x - stride;

  if (x == size-1)
    x4 = stride;
  else
    x4 = x + stride;
  y1 = y4 = y;

  if (y == 0)
    y2 = size-1 - stride;
  else
    y2 = y - stride;

  if (y == size-1)
    y3 = stride;
  else
    y3 = y + stride;
  
  x2 = x3 = x;
  field[x + y*size] = (field[x1 + size*y1] +
                 field[x2 + size*y2] +
                 field[x3 + size*y3] +
                 field[x4 + size*y4])/4.0f +
    scale * (.5f -  (float)rand()/(float)RAND_MAX);


  if (x == 0)
    field[size-1 + size*y] = field[x + size*y];
  if (y == 0)
    field[x + size*(size-1)] = field[x + size*y];
  
}

void BuildGrid( float *field, unsigned int size)
{
  int edgeCount = (size-1);
  int ptCt      = size;

  float ratio = pow(2, -H_FACT);
  float scale = 3;
  
  int stride = edgeCount/2;
  while (stride)
  {

    for (unsigned int i = stride ; i < edgeCount; i += stride)
    {
      for (unsigned int j = stride ; j < edgeCount; j += stride)
      {
        DoSquare(field, i, j, stride,scale, size);
        j+=stride;
      }
      i+=stride;
    }

    int oddline = 0;
    for (unsigned int i = 0 ; i < edgeCount; i+=stride)
    {
      oddline = (oddline?0:1);
      for (unsigned int j = 0 ; j < edgeCount; j+=stride)
      {
        if ((oddline) && !j) j+= stride; // offset a half step
        DoDiamond(field, i, j, stride, scale, size);
        j+= stride;
      }
    }
    stride = stride /2;
    scale *= ratio;
  }
}


int main(int argc, char ** argv)
{
  QApplication app(argc, argv);
  srand(time(0));
  HEIGHT_FIELD[0] =
    HEIGHT_FIELD[FIELD_SIZE-1] =
    HEIGHT_FIELD[FIELD_SIZE*(FIELD_SIZE-1)] =
    HEIGHT_FIELD[FIELD_SIZE*FIELD_SIZE-1] = 3;
  BuildGrid((float *)HEIGHT_FIELD, FIELD_SIZE);
  BuildClouds();
  MyGL qgl;
  qgl.resize(300, 300);
  qgl.show();
  app.exec();
  return 0;
}

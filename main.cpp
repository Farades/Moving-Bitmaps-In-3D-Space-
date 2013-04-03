#include<windows.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <GL/glu.h>
#include <GL/gl.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <iostream>

#define ANGLE_ROTATION 0.2
#define CALL_TIMER     30
#define TEX_IMAGE_NAME "D:/workspace/Moving-Bitmaps-In-3D-Space-/Star.jpg"

float WinWidth       = 900.0;
float WinHeight      = 900.0;

GLuint	texture[1];		// Место для одной текстуры

GLfloat	x_rotation = 50.;
GLfloat	y_rotation = 50.;
GLfloat	z_rotation = 50.;

BOOL    twinkle = true;                        // Twinkling Stars (Вращающиеся звезды)
const int  num = 50;                         // Количество рисуемых звезд

typedef struct                          // Создаём структуру для звезд
{
        int r, g, b;                    // Цвет звезды
        GLfloat dist;                   // Расстояние от центра
        GLfloat angle;                  // Текущий угол звезды
}
stars;                                  // Имя структуры - Stars
stars star[num];                        // Делаем массив 'star' длинной 'num',

GLfloat zoom=-15.0f;                    // Расстояние от наблюдателя до звезд
GLfloat tilt=90.0f;                     // Начальный угол
GLfloat spin;                           // Для вращения звезд

GLuint  loop;                           // Используется для циклов

//-----------------------------------------------------------------------------
//Таймер вращения
static void timer_rotation(int value = 0)
{
    x_rotation += ANGLE_ROTATION;
    y_rotation += ANGLE_ROTATION;
    z_rotation += ANGLE_ROTATION;

  /* send redisplay event */
  glutPostRedisplay();

  /* call this function again in 30 milliseconds */
  glutTimerFunc(CALL_TIMER, timer_rotation, 0);
}

//-----------------------------------------------------------------------------
//Функция загрузки изображения текстуры
void Load_Tex_Image()
{
    int width, height, bpp;

    ilLoad(IL_JPG, reinterpret_cast<const ILstring>(TEX_IMAGE_NAME));
    int err = ilGetError();                          // Считывание кода ошибки
    if (err != IL_NO_ERROR)
    {
        const char* strError = iluErrorString(err);  // Считываем строку ошибки
        std::cout << "Error load texture image: " << strError << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "Load texture image completed!" << std::endl;
        width  = ilGetInteger(IL_IMAGE_WIDTH);
        height = ilGetInteger(IL_IMAGE_HEIGHT);
        bpp    = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
        std::cout << "width:  "<< width << std::endl << "height: "
                  << height << std::endl << "bpp:    " << bpp << std::endl;
    }

    unsigned char* data = ilGetData();
    unsigned int type;

    switch (bpp) {
    case 1:
      type  = GL_RGB8;
      break;
    case 3:
      type = GL_RGB;
      break;
    case 4:
      type = GL_RGBA;
      break;
    }
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0,
    GL_RGB, GL_UNSIGNED_BYTE, data);
}

//-----------------------------------------------------------------------------
//Функция инициализации
void init()
{
    //Инициализация DevIL
    ilInit();
    iluInit();
    ilutInit();

    Load_Tex_Image();

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glViewport( 0, 0, WinWidth, WinHeight );

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glEnable(GL_BLEND);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 45.0f, (GLfloat)WinWidth/(GLfloat)WinHeight, 0.1f, 100.0f );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (loop = 0; loop < num; loop++)
    {
        star[loop].angle=0.0f;
        star[loop].dist=(float(loop)/num)*5.0f;
        // Присваиваем star[loop] случайное значение (красный)
        star[loop].r=rand()%256;
        // Присваиваем star[loop] случайное значение (зеленый)
        star[loop].g=rand()%256;
        // Присваиваем star[loop] случайное значение (голубой)
        star[loop].b=rand()%256;
    }
}


//-----------------------------------------------------------------------------
//Функция рисования сцены
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    for (loop = 0; loop < num; loop++)
    {
        glLoadIdentity();
        glTranslatef(0.0f,0.0f,zoom);
        glRotatef(tilt,1.0f,0.0f,0.0f);
        glRotatef(star[loop].angle,0.0f,1.0f,0.0f);
        glTranslatef(star[loop].dist,0.0f,0.0f);
        glRotatef(-star[loop].angle,0.0f,1.0f,0.0f);
        glRotatef(-tilt,1.0f,0.0f,0.0f);

        if (twinkle)
        {
            glColor4ub(star[(num-loop)-1].r,star[(num-loop)-1].g,star[(num-loop)-1].b,255);
            glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,-1.0f, 0.0f);
                glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,-1.0f, 0.0f);
                glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, 0.0f);
                glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
            glEnd();
        }

        glRotatef(spin,0.0f,0.0f,1.0f);
        glColor4ub(star[loop].r,star[loop].g,star[loop].b,255);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,-1.0f, 0.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,-1.0f, 0.0f);
            glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, 0.0f);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
        glEnd();

        spin += 0.01f;
        star[loop].angle+=float(loop)/num;
        star[loop].dist-=0.01f;
        if (star[loop].dist<0.0f)
        {
            star[loop].dist+=5.0f;
            star[loop].r=rand()%256;
            star[loop].g=rand()%256;
            star[loop].b=rand()%256;
        }
    }

    glFlush();
}

int main(int argc, char ** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WinWidth, WinHeight);
    glutInitWindowPosition(100, 70);
    glutCreateWindow("Texture_test");
    glutDisplayFunc(display);
    init();
    timer_rotation();
    glutMainLoop();
}

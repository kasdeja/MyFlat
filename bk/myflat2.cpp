#define GL_GLEXT_PROTOTYPES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>

// światło korytarz
// lightPos = vec3(lightModelViewMatrix * vec4(125.0 + 50.0, 260.0, 80.0 + 350.0, 1.0));

// światło lampa stoją w małym
// lightPos = vec3(lightModelViewMatrix * vec4(600.0, 172.0, 14.0, 1.0));

// światło centralne w małym
// lightPos = vec3(lightModelViewMatrix * vec4(473.0, 260.0, 137.0, 1.0));

// światło w dużym
// lightPos = vec3(lightModelViewMatrix * vec4(700.0, 260.0, 800.0, 1.0));

// mały wariant 2
//lightPos = vec3(lightModelViewMatrix * vec4(590.0, 172.0, 14.0, 1.0));  \

const char *vertexShader =
"                                                                           \
varying vec3 N;                                                             \
varying vec3 v;                                                             \
varying vec3 lightPos;                                                      \
uniform mat4 lightModelViewMatrix;                                          \
                                                                            \
void main()                                                                 \
{                                                                           \
    v = vec3(gl_ModelViewMatrix * gl_Vertex);                               \
    lightPos = vec3(lightModelViewMatrix * vec4(175.0, 260.0, 100.0, 1.0));  \
    N = normalize(gl_NormalMatrix * gl_Normal);                             \
                                                                            \
    gl_FrontColor = gl_Color;                                               \
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;                 \
}                                                                           \
";

const char *fragmentShader =
"                                                                                                 \n\
varying vec3 N;                                                                                   \n\
varying vec3 v;                                                                                   \n\
varying vec3 lightPos;                                                                            \n\
                                                                                                  \n\
vec3 Uncharted2ToneMapping(vec3 color)                                                            \n\
{                                                                                                 \n\
    float A = 0.15;                                                                               \n\
    float B = 0.50;                                                                               \n\
    float C = 0.10;                                                                               \n\
    float D = 0.20;                                                                               \n\
    float E = 0.02;                                                                               \n\
    float F = 0.30;                                                                               \n\
    float W = 11.2;                                                                               \n\
    float exposure = 2.0;                                                                         \n\
    float gamma = 2.2;                                                                            \n\
                                                                                                  \n\
    color *= exposure;                                                                            \n\
    color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;  \n\
    float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;            \n\
    color /= white;                                                                               \n\
    color = pow(color, vec3(1.0 / gamma));                                                        \n\
    return color;                                                                                 \n\
}                                                                                                 \n\
                                                                                                  \n\
void main()                                                                                       \n\
{                                                                                                 \n\
    if (gl_Color.x < 0.99)                                                                        \n\
    {                                                                                             \n\
        vec3 vl = v - lightPos;                                                                   \n\
        vec3 L = normalize(lightPos - v);                                                         \n\
        float Idiff = max(dot(N, L), 0.0);                                                        \n\
        float dist = 20000.0 / dot(vl, vl);                                                       \n\
                                                                                                  \n\
        vec4 color1 = gl_Color * dist * Idiff + gl_Color * 0.005;                                 \n\
        vec3 tm = Uncharted2ToneMapping(vec3(color1.x, color1.y, color1.z));                      \n\
        gl_FragColor = vec4(tm.x, tm.y, tm.z, 1.0);                                               \n\
    }                                                                                             \n\
    else                                                                                          \n\
    {                                                                                             \n\
        gl_FragColor = gl_Color;                                                                  \n\
    }                                                                                             \n\
}                                                                                                 \n\
";

//    gl_FragColor = vec4(sqrt(color1.x), sqrt(color1.y), sqrt(color1.z), 1.0);

struct Vec3
{
    float x;
    float y;
    float z;
};

class Matrix
{
public:
    float m[3][3];

    Matrix()
    {
        for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < 3; j++)
                m[i][j] = 0;
        }
    }

    Matrix& Identity()
    {
        for(int i = 0; i < 3; i++)
        {
            m[i][i] = 1.0f;
        }

        return *this;
    }

    Matrix& RotX(float a)
    {
        m[0][0] = 1.0f;
        m[1][1] = m[2][2] = cos(a);
        m[2][1] = sin(a);
        m[1][2] = -sin(a);

        return *this;
    }

    Matrix& RotY(float a)
    {
        m[1][1] = 1.0f;
        m[0][0] = m[2][2] = cos(a);
        m[0][2] = sin(a);
        m[2][0] = -sin(a);

        return *this;
    }

    float* operator[](const int j)
    {
        return m[j];
    }

    Matrix operator*(const Matrix &a)
    {
        Matrix c;

        for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                c[i][j] = 0;
                for(int n = 0; n < 3; n++)
                    c[i][j] += this->m[n][i] * a.m[j][n];
            }
        }

        return c;
    }

    Matrix& operator*=(const Matrix &a)
    {
        *this = *this * a;
        return *this;
    }
};

class Scene
{
public:
    void Draw(float y);
    void DrawGarderoba();
    void DrawMaly();

    void DrawBiurko(float ox, float oy, float oz, float w, float d, float h);
    void DrawRegal(float ox, float oy, float oz, int rot);
    void DrawKrzeslo(float ox, float oy, float oz, int rot);
    void DrawLampa(float ox, float oy, float oz);
    void DrawSzuflada(float ox, float oy, float oz, float w, float h, float d);

private:
    void DrawBox(float ox, float oy, float oz, float w, float h, float d);
    void DrawBox2(float ox, float oy, float oz,
                  float wl, float dl, float wh, float dh,
                  float sx, float sz, float h);
    void DrawDoors(float ox, float oy, float oz, int rot, int ns, bool left);
    void DrawFrame(
        float ox, float oy, float oz, int rot,
        float width, float height, float depth,
        float top, float bottom, float left, float right);
};

void Scene::Draw(float y)
{
    glColor3f(0.5f, 0.5f, 0.5f);

    DrawBox(  0.0f, -10.0f,   0.0f, 940.0f, 10.0f,   870.0f);   // podłoga

    if (y < 271.0)
    {
        DrawBox(  0.0f, 271.0f,   0.0f, 940.0f, 10.0f,   870.0f);   // sufit
    }

    DrawBox(  0.0f,   0.0f, -10.0f, 690.0f, 271.0f,   10.0f);   // ściana tył, wspólna z mniejszym pokojem i garderobą

    DrawBox(-10.0f,   0.0f,   0.0f,  10.0f, 271.0f,  333.6f);   // ściana lewa, garderoba, tył szafy w przedpokoju

    DrawBox(  0.0f,   0.0f,   0.0f,  55.7f, 271.0f,   30.3f);   // pion w garderobie
    DrawBox(250.2f,   0.0f,   0.0f,   8.0f, 271.0f,  179.5f);   // ściana między garderobą a mniejszym pokojem
    DrawBox(250.2f,   0.0f, 267.9f,   8.0f, 271.0f,    6.5f);   // ściana wejściowa do mniejszego pokoju z otworem na drzwi
    DrawBox(250.2f, 209.0f, 179.5f,   8.0f,  62.0f,   88.4f);

    DrawBox(  0.0f, 209.0f, 164.6f, 250.2f,   62.0f,   8.0f);   // ściana wejściowa do garderoby z otworem na drzwi
    DrawBox(  0.0f,   0.0f, 164.6f, 119.8f,  210.0f,   8.0f);
    DrawBox(213.6f,   0.0f, 164.6f,  37.1f,  210.0f,   8.0f);

    DrawBox(688.2f,   0.0f,   0.0f,  40.0f, 271.0f,   50.0f);   // ściana małego pokoju z oknem
    DrawBox(688.2f,   0.0f, 224.9f,  40.0f, 271.0f,   49.5f);
    DrawBox(688.2f,   0.0f,  50.0f,  40.0f,  87.5f,  174.9f);
    DrawBox(688.2f, 232.5f,  50.0f,  40.0f,  38.5f,  174.9f);

    DrawBox(250.2f,   0.0f, 274.4f, 438.0f, 271.0f,    8.0f);   // ściana między mniejszym pokojem a kuchnią
    DrawBox(250.2f,   0.0f, 282.4f,  54.5f, 271.0f,   75.0f);   // pion korytarz / kuchnia

    DrawBox(304.7f,   0.0f, 479.4f, 423.5f, 271.0f,    8.0f);   // ściana między kuchnią a dużym pokojem

    DrawBox(688.2f,   0.0f, 274.4f,  40.0f, 271.0f,   68.0f);   // ściana w kuchni z oknem
    DrawBox(688.2f,   0.0f, 429.4f,  40.0f, 271.0f,   50.0f);
    DrawBox(688.2f,   0.0f, 342.4f,  40.0f,  87.5f,   87.0f);
    DrawBox(688.2f, 232.5f, 342.4f,  40.0f,  38.5f,   87.0f);

    DrawBox(296.7f,   0.0f, 357.4f,   8.0f, 271.0f,   19.0f);   // ściana wejściowa do kuchni z drzwiami
    DrawBox(296.7f, 209.0f, 376.4f,   8.0f,  62.0f,   88.0f);
    DrawBox(296.7f,   0.0f, 464.4f,   8.0f, 271.0f,   29.0f);

    DrawBox(103.7f,   0.0f, 850.4f, 772.0f, 271.0f,    8.0f);   // ściana przód, wspólna z dużym pokojem i łazienką

    DrawBox(  0.0f,   0.0f, 325.6f, 103.7f, 271.0f,    8.0f);   // ściana od szafy w przedpokoju
    DrawBox( 95.7f,   0.0f, 333.6f,   8.0f, 271.0f,  524.8f);   // ściana z drzwiami wejściowymi

    DrawBox(296.7f,   0.0f, 580.4f,   8.0f, 271.0f,  270.0f);   // ściana między łazienką a salonem
    DrawBox(296.7f, 209.0f, 493.4f,   8.0f,  62.0f,   87.0f);

    DrawBox(103.7f,   0.0f, 586.4f,  68.0f, 271.0f,    8.0f);   // ściana z otworem na drzwi do lazienki
    DrawBox(171.7f, 209.0f, 586.4f,  89.0f,  62.0f,    8.0f);
    DrawBox(260.7f,   0.0f, 586.4f,  36.0f, 271.0f,    8.0f);

    DrawBox(103.7f,   0.0f, 794.4f,  50.5f, 271.0f,   56.0f);   // pion w łazience

    DrawBox(875.7f,   0.0f, 766.4f,  40.0f, 271.0f,   84.0f);   // ściana z oknem w dużym pokoju
    DrawBox(875.7f,   0.0f, 447.4f,  40.0f,  87.5f,  319.0f);
    DrawBox(875.7f, 232.5f, 447.4f,  40.0f,  38.5f,  319.0f);

    DrawBox(814.2f,   0.0f, 447.4f,  61.5f,  87.5f,   40.0f);
    DrawBox(728.2f, 232.5f, 447.4f, 147.5f,  38.5f,   40.0f);

    glColor3f(0.8f, 0.3f, 0.0f);
    DrawBox(258.2f,  -1.0f,   0.0f, 430.0f, 1.1f, 274.4f);      // parkiet w małym pokoju
    DrawBox(304.7f,  -1.0f, 487.4f, 571.0f, 1.1f, 363.0f);      // parkiet w dużym pokoju
    DrawBox(728.2f,  -1.0f, 466.4f,  86.0f, 1.1f,  21.0f);

    // Drzwi wejściowe
    glColor3f(0.06f, 0.025f, 0.011f);
    DrawBox(103.7f,   0.0f, 383.6f,   0.5f, 209.5f,  102.5f);
    glColor3f(0.56f, 0.2f, 0.082f);
    DrawBox(104.2f,   0.0f, 385.6f,   0.5f, 206.5f,   97.5f);

    DrawDoors(250.2f, 0.0f, 176.5f,  0, 2,  true);    // drzwi mały pokój
    DrawDoors(296.7f, 0.0f, 374.5f,  0, 2,  true);    // drzwi kuchnia
    DrawDoors(296.7f, 0.0f, 490.4f,  0, 2, false);    // drzwi duży pokój

    DrawDoors(171.7f, 0.0f, 594.4f,  90, 1, false);    // drzwi łazienka
    DrawDoors(213.3f, 0.0f, 164.6f, 270, 0,  true);    // drzwi garderoba

    // parapety
    glColor3f(0.7f, 0.6f, 0.3f);
    DrawBox(686.2f, 87.5f,  48.5f,  24.0f,  2.0f,  177.9f);     // mały
    DrawBox(686.2f, 87.5f,  340.9f,  24.0f,  2.0f,  90.0f);     // kuchnia
    DrawBox(873.7f, 87.5f,  463.4f,  24.0f,  2.0f,  304.5f);    // duży
    DrawBox(814.2f, 87.5f,  466.4f,  59.5f,  2.0f,   23.0f);

    // Okno mały pokój
    glColor3f(0.56f, 0.2f, 0.082f);

    DrawFrame(
        709.2f,  87.5f, 50.0f, 0,
         84.0f, 145.0f, 7.5f,
         4.0f,    5.0f, 4.0f, 2.0f);

    DrawFrame(
        709.2f,  87.5f,134.0f, 0,
         90.9f, 145.0f,  7.5f,
         4.0f,    5.0f,  2.0f, 4.0f);

    DrawFrame(
        707.5f,  92.5f, 54.0f, 0,
         79.0f, 136.0f, 9.0f,
         7.5f,    7.5f, 7.5f, 4.0f);

    DrawFrame(
        707.5f,  92.5f,134.5f, 0,
         86.9f, 136.0f, 9.0f,
         7.5f,    7.5f, 7.5f, 7.5f);

    // Okno kuchnia
    DrawFrame(
        709.2f,  87.5f, 342.4f, 0,
         87.0f, 145.0f, 7.5f,
         4.0f,    5.0f, 2.0f, 4.0f);

    DrawFrame(
        707.5f,  92.5f, 344.4f, 0,
         82.0f, 136.0f, 9.0f,
         7.5f,    7.5f, 7.5f, 4.0f);

    // Okno duży pokój
    DrawFrame(
        896.7f,  87.5f, 466.4f, 0,
        150.0f, 145.0f,   7.5f,
          4.0f,   5.0f,   8.0f, 2.0f);

    DrawFrame(
        896.7f,  87.5f, 616.4f, 0,
        150.0f, 145.0f,   7.5f,
          4.0f,   5.0f,   2.0f, 4.0f);

    DrawFrame(
        895.0f,  92.5f, 474.4f, 0,
        140.0f, 136.0f,   9.0f,
          7.5f,   7.5f,   7.5f, 7.5f);

    DrawFrame(
        895.0f,  92.5f, 618.4f, 0,
        144.0f, 136.0f,   9.0f,
          7.5f,   7.5f,   7.5f, 7.5f);

    DrawFrame(
        814.2f,  87.5f, 466.4f, 90,
         89.7f, 145.0f,   7.5f,
          4.0f,   5.0f,   5.0f, 16.5f);

    DrawFrame(
        819.2f,  92.5f, 468.1f, 90,
         68.2f, 136.0f,   9.0f,
          7.5f,   7.5f,   7.5f, 7.5f);

    DrawFrame(
        728.2f,   0.0f, 466.4f, 90,
         86.0f, 232.5f,   7.5f,
          3.0f,   3.0f,   3.0f, 3.0f);

    DrawFrame(
        731.2f,   3.0f, 468.1f, 90,
         80.0f, 226.5f,   9.0f,
          7.5f,   7.5f,   7.5f, 7.5f);


    // kaloryfery, klimatyzatory, szyny
    glColor3f(0.8f, 0.8f, 0.8f);

    DrawBox(678.6f, 15.5f,   79.5f,  6.0f, 60.0f, 120.9f);   // mały pokój kaloryfer
    DrawBox(571.2f, 234.0f, 254.2f, 83.5f, 28.0f,  20.0f);   // mały pokój klimatyzator
    DrawBox(678.6f, 15.5f,  365.9f,  6.0f, 60.0f,  50.5f);   // kuchnia kaloryfer

    DrawBox(866.1f, 15.5f,  532.4f,  6.0f, 60.0f, 180.0f);   // duży pokój kaloryfer
    DrawBox(564.7f, 234.0f, 487.4f, 83.5f, 28.0f,  20.0f);   // duży pokój klimatyzator

    DrawBox(673.7f, 270.0f,   5.0f,  4.5f, 1.0f,  264.4f);  // szyna mały pokój

    DrawBox(860.7f, 270.0f, 497.9f,   4.5f, 1.0f,  347.5f);  // szyna duży pokój
    DrawBox(656.2f, 270.0f, 497.9f, 204.5f, 1.0f,    4.5f);

    // zasłony
    glColor3f(0.05f, 0.05f, 0.05f);

    DrawBox(673.7f, 6.5f,   3.0f,  1.0f, 259.5f, 60.0f); // zasłona lewa mały pokój
    DrawBox(673.7f, 6.5f, 221.4f,  1.0f, 259.5f, 50.0f); // zasłona prawa mały pokój

    DrawBox(860.7f, 6.5f, 756.4f,  1.0f, 259.5f, 45.0f); // zasłona prawa duży pokój
    DrawBox(860.7f, 6.5f, 803.4f,  1.0f, 259.5f, 45.0f);

    DrawBox(654.2f, 6.5f, 502.4f, 72.0f, 259.5f,  1.0f); // zasłona lewa duży pokój

    // szafa przedpokój
    glColor3f(0.56f, 0.2f, 0.082f);
    DrawBox(  0.0f,   0.0f, 323.7f, 51.0f, 271.0f,    1.9f);
    DrawBox(  0.0f,   0.0f, 172.6f, 51.0f, 271.0f,    1.9f);

    DrawBox(  0.0f,   0.0f, 281.7f, 51.0f, 229.1f,    1.9f);
    DrawBox(  0.0f,   0.0f, 246.4f, 51.0f, 229.1f,    1.9f);

    DrawBox(  0.0f, 229.1f, 174.5f, 51.0f,   1.9f,  149.2f);

    DrawBox(  0.0f,  34.0f, 174.5f, 51.0f,   1.9f,  107.2f);
    DrawBox(  0.0f,  63.9f, 248.3f, 51.0f,   1.9f,   75.4f);
    DrawBox(  0.0f, 102.5f, 248.3f, 51.0f,   1.9f,   75.4f);
    DrawBox(  0.0f, 133.0f, 248.3f, 51.0f,   1.9f,   75.4f);
    DrawBox(  0.0f, 166.0f, 248.3f, 51.0f,   1.9f,   75.4f);
    DrawBox(  0.0f, 196.5f, 174.5f, 51.0f,   1.9f,  149.2f);

    DrawFrame(
         51.0f,   0.0f, 172.6f, 0,
        153.0f, 271.0f,   9.0f,
          1.9f,   1.9f,   1.9f, 1.9f);

    glColor3f(0.8f, 0.8f, 0.8f);
    DrawBox( 26.5f, 189.0f, 174.5f, 3.0f,   3.0f,  71.9f);
    DrawBox( 59.9f, 264.6f, 174.5f, 0.1f,   4.5f, 149.2f);

    //DrawBox( 55.9f, 264.6f, 174.5f, 0.1f,   4.5f, 149.2f);
    //DrawBox( 51.9f, 264.6f, 174.5f, 0.1f,   4.5f, 149.2f);

    DrawBox( 52.0f, 1.9f, 174.5f, 7.0f,   0.2f, 149.2f);

    // meble
    DrawGarderoba();
    DrawMaly();
}

void Scene::DrawMaly()
{
    bool lozko;
    Vec3 lozkoPos;
    Vec3 biurkoPos;
    Vec3 regalPos;
    Vec3 lampaPos;
    Vec3 krzeslo1Pos;
    Vec3 krzeslo2Pos;
    int  krzeslo1Rot;
    int  krzeslo2Rot;
    int  regalRot;
    int  wariant = 2;

    if (wariant == 0)
    {
        lozko = false;
        lozkoPos = { 208.0f, 0.0f, 138.4f };
        biurkoPos = { 99.5f, 0.0f, 0.5f };
        krzeslo1Pos = { 143.0f, 0.0f, 87.0f };
        krzeslo2Pos = { 314.0f, 0.0f, 48.0f };
        krzeslo1Rot = 0;
        krzeslo2Rot = 180;
        regalPos = { 1.0f, 0.0f, 0.5f };
        regalRot = 0;
        lampaPos = { 320.0f, 0.0f, 4.0f };
    }
    else if (wariant == 1)
    {
        lozko = true;
        lozkoPos = { 1.0f, 0.0f, 1.0f };
        biurkoPos = { 253.0f, 0.0f, 194.0f };
        krzeslo1Pos = { 0.0f, 0.0f, 0.0f };
        krzeslo2Pos = { 368.0f, 0.0f, 187.0f };
        krzeslo1Rot = -1;
        krzeslo2Rot = 180;
        regalPos = { 333.0f, 0.0f, 0.5f };
        regalRot = 0;
        lampaPos = { 290.0f, 0.0f, 4.0f };
    }
    else if (wariant == 2)
    {
        lozko = true;
        lozkoPos = { 1.0f, 0.0f, 1.0f };
        biurkoPos = { 253.0f, 0.0f, 194.0f };
        krzeslo1Pos = { 405.0f, 0.0f, 48.0f };
        krzeslo2Pos = { 368.0f, 0.0f, 187.0f };
        krzeslo1Rot = 180;
        krzeslo2Rot = 180;
        regalPos = { 236.0f, 0.0f, 273.5f };
        regalRot = 180;
        lampaPos = { 320.0f, 0.0f, 4.0f };
    }


    // Mały pokój
    glPushMatrix();
    glTranslatef(258.2f, 0.0f, 0.0f);

    // Dywan
    glPushMatrix();
    glTranslatef(83.0f, 0.0f, 29.0f);

    for(int m = 0; m < 4; m++)
    {
        for(int n = 0; n < 6; n++)
        {
            constexpr float dx = 291.5f / 6.0f;
            constexpr float dy = 200.0f / 4.0f;

            if ((m ^ n) & 1)
                glColor3f(0.15f, 0.15f, 0.15f);
            else
                glColor3f(0.05f, 0.05f, 0.05f);

            DrawBox(n * dx, 0.1f, m * dy, dx, 0.2f, dy);
        }
    }

    glPopMatrix();

    // Materac / łózko
    glPushMatrix();
    glTranslatef(lozkoPos.x, lozkoPos.y, lozkoPos.z);

    if (lozko)
    {
        glColor3f(0.2f, 0.2f, 0.2f);
        DrawBox(0.0f, 0.1f, 0.0f, 200.0f, 24.0f, 135.0f);

        glColor3f(0.03f, 0.07f, 0.17f);
        DrawBox(0.0f, 24.1f, 0.0f, 200.0f, 26.5f, 135.0f);
    }
    else
    {
        glColor3f(0.03f, 0.07f, 0.17f);
        DrawBox(0.0f, 0.1f, 0.0f, 200.0f, 26.5f, 135.0f);
    }

    glPopMatrix();

    DrawBiurko(biurkoPos.x, biurkoPos.y, biurkoPos.z, 160.0f, 80.0f, 69.0f);
    DrawRegal(regalPos.x, regalPos.y, regalPos.z, regalRot);

    if (krzeslo1Rot != -1)
        DrawKrzeslo(krzeslo1Pos.x, krzeslo1Pos.y, krzeslo1Pos.z, krzeslo1Rot);

    if (krzeslo2Rot != -1)
        DrawKrzeslo(krzeslo2Pos.x, krzeslo2Pos.y, krzeslo2Pos.z, krzeslo2Rot);

    DrawLampa(lampaPos.x, lampaPos.y, lampaPos.z);

    // Koniec
    glPopMatrix();
}

void Scene::DrawLampa(float ox, float oy, float oz)
{
    glPushMatrix();
    glTranslatef(ox, oy, oz);

    glColor3f(0.02f, 0.02f, 0.02f);

    DrawBox( 0.0f, 0.0f,  0.0f,  26.0f,   2.0f, 26.0f);
    DrawBox(12.0f, 2.0f, 12.0f,   2.0f, 155.5f,  2.0f);

    DrawBox2(11.8f, 155.5f, 11.8f,
              2.4f, 2.4f, 10.0f, 10.0f,
             -3.8f, -3.8f, 9.0f);

    glColor3f(1.0f, 1.0f, 1.0f);

    DrawBox2( 8.0f, 164.5f, 8.0f,
             10.0f, 10.0f, 26.0f, 26.0f,
             -8.0f, -8.0f, 12.0f);

    glPopMatrix();
}

void Scene::DrawSzuflada(float ox, float oy, float oz, float w, float h, float d)
{
    float th = 1.8f;

    glColor3f(0.7f, 0.55f, 0.3f);
    DrawBox(ox + th, oy, oz + th, w - 2 * th, 0.2, d - 2 * th);

    glColor3f(0.56f, 0.2f, 0.082f);
    DrawBox(ox,          oy, oz, th, h, d - th);
    DrawBox(ox + w - th, oy, oz, th, h, d - th);

    DrawBox(ox, oy, oz, w, h, th);
    DrawBox(ox, oy, oz + d - th, w, h - 4.0f, th);

    glColor3f(0.9f, 0.9f, 0.9f);
    DrawBox(ox, oy + h - 4.0f, oz + d - th, w, 0.2f, th);
    DrawBox(ox, oy + h - 0.2f, oz + d - th, w, 0.2f, th);
    DrawBox(ox, oy + h - 3.8f, oz + d - th, w, 3.6f, 0.2);
}

void Scene::DrawKrzeslo(float ox, float oy, float oz, int rot)
{
    glPushMatrix();
    glTranslatef(ox, oy, oz);
    glRotatef(rot, 0.0f, 1.0f, 0.0f);

    glColor3f(0.05f, 0.05f, 0.05f);

    DrawBox2(0.0f, 0.0f,    0.0f,    3.0f, 1.5f, 3.0f, 1.5f,     0.0f,  10.0f, 49.0f);
    DrawBox2(0.0f, 0.0f,   40.0f,    3.0f, 1.5f, 3.0f, 1.5f,     0.0f, -10.0f, 49.0f);
    DrawBox (0.0f, 47.5f,  10.0f,    3.0f, 1.5f, 21.5f);

    DrawBox2(51.5f, 0.0f,    0.0f,   3.0f, 1.5f, 3.0f, 1.5f,     0.0f,  10.0f, 49.0f);
    DrawBox2(51.5f, 0.0f,   40.0f,   3.0f, 1.5f, 3.0f, 1.5f,     0.0f, -10.0f, 49.0f);
    DrawBox (51.5f, 47.5f,  10.0f,   3.0f, 1.5f, 21.5f);

    DrawBox (0.0f, 41.5f,   10.0f,  54.5f, 1.5f, 1.5f);
    DrawBox (0.0f, 41.5f,   30.0f,  54.5f, 1.5f, 1.5f);

    DrawBox ( 3.0f, 43.0f,    8.0f,   3.0f, 1.5f, 30.0f);
    DrawBox (48.5f, 43.0f,    8.0f,   3.0f, 1.5f, 30.0f);

    DrawBox2( 3.0f, 44.5f,   36.5f,    3.0f, 1.5f, 3.0f, 1.5f,     0.0f,  10.0f, 38.0f);
    DrawBox2(48.5f, 44.5f,   36.5f,    3.0f, 1.5f, 3.0f, 1.5f,     0.0f,  10.0f, 38.0f);

    glColor3f(0.02f, 0.02f, 0.02f);

    DrawBox ( 3.5f, 44.5f,    3.0f,  47.5f, 1.5f, 33.0f);
    DrawBox2( 2.5f, 48.5f,   36.5f,  49.5f, 1.5f, 49.5f, 1.5f,     0.0f,  9.0f, 35.0f);

    glPopMatrix();
}

void Scene::DrawBiurko(float ox, float oy, float oz, float w, float d, float h)
{
    glPushMatrix();
    glTranslatef(ox, oy, oz);

    glColor3f(0.14f, 0.05f, 0.01f);
    DrawBox(0.0f, h, 0.0f, w, 2.7f, d);

    glColor3f(0.3f, 0.3f, 0.3f);

    DrawBox(9.5f, 0.1f, (d - 70.0f) / 2.0f,  6.0f, 2.0f, 70.0f);
    DrawBox(9.5f, h - 1.0f, (d - 35.0f) / 2.0f,  6.0f, 1.0f, 35.0f);
    DrawBox(9.5f, 2.1f, (d - 6.0f) / 2.0f,  6.0f, 50.0f, 6.0f);
    DrawBox(10.0f, 52.1f, (d - 5.0f) / 2.0f,  5.0f, h - 53.1f, 5.0f);

    DrawBox(w - 15.5f, 0.1f, (d - 70.0f) / 2.0f,  6.0f, 2.0f, 70.0f);
    DrawBox(w - 15.5f, h - 1.0f, (d - 35.0f) / 2.0f,  6.0f, 1.0f, 35.0f);
    DrawBox(w - 15.5f, 2.1f, (d - 6.0f) / 2.0f,  6.0f, 50.0f, 6.0f);
    DrawBox(w - 15.0f, 52.1f, (d - 5.0f) / 2.0f,  5.0f, h - 53.1f, 5.0f);

    DrawBox(9.5f, h - 5.0f, (d - 10.0f) / 2.0f,  w - 19.0f, 5.0f, 10.0f);

    glPopMatrix();
}

void Scene::DrawRegal(float ox, float oy, float oz, int rot)
{
    glPushMatrix();
    glTranslatef(ox, oy, oz);
    glRotatef(rot, 0.0f, 1.0f, 0.0f);

    glColor3f(0.7f, 0.55f, 0.3f);

    DrawBox( 0.0f, 0.1f, 0.0f,  1.9f, 202.0f, 28.0f);
    DrawBox(78.2f, 0.1f, 0.0f,  1.9f, 202.0f, 28.0f);

    for(int n = 0; n < 7; n++)
    {
        DrawBox(1.9f, 8.1f + (n * 192.1f) / 6.0f, 0.1f, 76.3f, 1.9f, 27.4f);
    }

    DrawBox( 1.9f, 0.1f, 23.4f,  76.3f,   8.0f, 1.9f);
    DrawBox( 1.9f, 0.1f,  0.0f,  76.3f, 202.0f, 0.1f);

    glPopMatrix();
}

void Scene::DrawFrame(
    float ox, float oy, float oz, int rot,
    float width, float height, float depth,
    float top, float bottom, float left, float right)
{
    glPushMatrix();
    glTranslatef(ox, oy, oz);
    glRotatef(rot, 0.0f, 1.0f, 0.0f);

    DrawBox(0.0f, 0.0f, 0.0f, depth, height, left);
    DrawBox(0.0f, 0.0f, width - right, depth, height, right);
    DrawBox(0.0f, 0.0f, left, depth, bottom, width - left - right);
    DrawBox(0.0f, height - top, left, depth, top, width - left - right);

    glPopMatrix();
}

void Scene::DrawDoors(float ox, float oy, float oz, int rot, int ns, bool left)
{
    glColor3f(0.56f, 0.2f, 0.082f);

    glPushMatrix();
    glTranslatef(ox, oy, oz);
    glRotatef(rot, 0.0f, 1.0f, 0.0f);

    DrawBox(-1.75f,   0.0f,  0.0f, 11.5f,  209.5f,  6.0f);
    DrawBox( 0.25f,   0.0f,  6.0f,  7.5f,  202.3f,  1.2f);

    DrawBox(-1.75f,   0.0f, 88.5f, 11.5f,  209.5f,  6.0f);
    DrawBox( 0.25f,   0.0f, 87.3f,  7.5f,  202.3f,  1.2f);

    DrawBox(-1.75f, 203.5f,  6.0f, 11.5f,    6.0f, 88.5f);
    DrawBox( 0.25f, 202.3f,  6.0f,  7.5f,    1.2f, 88.5f);

    static float xxx = 90.0f;

    glTranslatef(10.5f, 0.0f, 2.0f);

    if (left)
    {
        glTranslatef(-2.0f, 0.0f, 90.5f);
        glRotatef(180.0f - xxx, 0.0f, 1.0f, 0.0f);
    }
    else
    {
        glRotatef(xxx, 0.0f, 1.0f, 0.0f);
    }

    glTranslatef(-3.0f, 0.0f, -2.0f);

    DrawBox(  0.0f,   0.0f,  6.0f,  4.0f,  203.5f, 12.5f);
    DrawBox(  0.0f,   0.0f, 76.0f,  4.0f,  203.5f, 12.5f);

    DrawBox(  0.4f,   0.0f, 18.5f,  3.2f,   19.0f, 57.5f);
    DrawBox(  0.4f, 191.0f, 18.5f,  3.2f,   12.5f, 57.5f);

    for(int n = 0; n < 4; n++)
    {
        DrawBox(1.4f, 19.0f + 51.67 * n, 18.5f, 1.2f, 17.0f, 57.5f);
    }

    for(int n = 0; n < 3; n++)
    {
        if (n >= (3 - ns))
            glColor3f(0.4f, 0.4f, 0.4f);
        DrawBox(1.9f, 36.0f + 51.67 * n, 18.5f, 0.2f, 34.67f, 57.5f);
    }

    glPopMatrix();
}

void Scene::DrawGarderoba()
{
    glColor3f(0.8f, 0.3f, 0.0f);
    DrawBox(0.0f,  -1.0f, 0.0f, 250.2f, 1.1f, 164.6f);          // parkiet

    glColor3f(0.9f, 0.9f, 0.9f);
    DrawBox(2.0f,   0.0f, 98.8f, 61.0f, 86.0f, 59.8f);         // suszarka

    DrawBox(60.7f,  0.0f, 2.0f, 40.0f, 32.5f, 61.0f);           // pudełko 1 dół przód
    DrawBox(103.7f, 0.0f, 2.0f, 40.0f, 32.5f, 61.0f);           // pudełko 2 dół przód

    // półki lewo
    glColor3f(0.56f, 0.2f, 0.082f);
    DrawBox(0.0f, 0.0f,  30.3f,  55.7f, 230.0f,   1.8f);        // bok prawy
    DrawBox(0.0f, 0.0f, 162.8f,  55.7f, 230.0f,   1.8f);        // bok lewy
    DrawBox(0.0f, 0.0f,  93.0f,  55.7f, 230.0f,   1.8f);        // srodek

    DrawBox(0.0f, 230.0f,  30.3f, 55.7f,   1.8f, 134.3f);       // wierzch
    DrawBox(0.0f,  90.0f,  94.8f, 55.7f,   1.8f,  68.0f);       // polka 1 nad pralką
    DrawBox(0.0f, 128.3f,  94.8f, 55.7f,   1.8f,  68.0f);       // polka 2 nad pralką
    DrawBox(0.0f, 166.6f,  94.8f, 55.7f,   1.8f,  68.0f);       // polka 3 nad pralką
    DrawBox(0.0f, 204.9f,  94.8f, 55.7f,   1.8f,  68.0f);       // polka 4 nad pralką
    DrawBox(0.0f,  36.5f,  30.3f, 55.7f,   1.8f,  68.0f);       // polka 1
    DrawBox(0.0f, 204.9f,  30.3f, 55.7f,   1.8f,  68.0f);       // polka 2

    // półki przód
    DrawBox( 55.7f,   0.0f,  0.0f,   1.8f, 230.0f,  50.0f);     // bok lewy
    DrawBox(162.9f,   0.0f,  0.0f,   1.8f, 230.0f,  50.0f);     // bok środek
    DrawBox(248.4f,   0.0f,  0.0f,   1.8f, 230.0f,  50.0f);     // bok prawy
    DrawBox( 55.7f, 230.0f,  0.0f, 194.4f,   1.8f,  50.0f);     // wierzch
    DrawBox( 57.5f,  36.5f,  0.0f, 105.4f,   1.8f,  50.0f);     // polka 1
    DrawBox( 57.5f,  79.3f,  0.0f, 105.4f,   1.8f,  50.0f);     // polka 2
    DrawBox( 57.5f, 101.3f,  0.0f, 105.4f,   1.8f,  50.0f);     // polka 3
    DrawBox( 57.5f, 138.6f,  0.0f, 105.4f,   1.8f,  50.0f);     // polka 4
    DrawBox( 57.5f, 172.9f,  0.0f, 105.4f,   1.8f,  50.0f);     // polka 5
    DrawBox( 57.5f, 204.9f,  0.0f, 105.4f,   1.8f,  50.0f);     // polka 6

    DrawBox(164.7f, 101.3f,  0.0f, 83.7f,   1.8f,  50.0f);      // polka 1 nad oponami
    DrawBox(164.7f, 138.6f,  0.0f, 83.7f,   1.8f,  50.0f);      // polka 2 nad oponami
    DrawBox(164.7f, 172.9f,  0.0f, 83.7f,   1.8f,  50.0f);      // polka 2 nad oponami
    DrawBox(164.7f, 204.9f,  0.0f, 83.7f,   1.8f,  50.0f);      // polka 4 nad oponami


    DrawBox(109.3f,   81.1f,  0.0f,   1.8f, 20.2f,  50.0f);     // podział półki z szufladami
    DrawSzuflada( 57.9f, 81.5f, 0.0f, 51.0f,   19.6f, 50.0f);
    DrawSzuflada(111.5f, 81.5f, 0.0f, 51.0f,   19.6f, 50.0f);

    // opony
    glColor3f(0.8f, 0.8f, 0.8f);
    for(int n = 0; n < 4; n++)
    {
        DrawBox(172.7f, 23.25f * n, 2.0f, 66.0f, 22.5f, 66.0f);       // opona
    }

    // półki prawo
    /*DrawBox(213.1f,   0.0f,   50.0f,  37.1f, 230.0f,   1.8f);      // bok lewy
    DrawBox(213.1f,   0.0f,  124.6f,  37.1f, 230.0f,   1.8f);      // bok środek
    DrawBox(213.1f,  45.0f,   88.2f,  37.1f, 154.0f,   1.8f);      // bok środek 2
    DrawBox(213.1f, 170.0f,  162.8f,  37.1f,  60.0f,   1.8f);      // bok prawy
    DrawBox(213.1f, 230.0f,   50.0f,  37.1f,   1.8f, 114.6f);      // wierzch

    DrawBox(213.1f,  45.0f,   51.8f,  37.1f,   1.8f,  72.8f);      // polka 1
    DrawBox(213.1f, 168.33333f,  126.4f,  37.1f,   1.8f,  38.2f);      // polka 2
    DrawBox(213.1f, 199.16666f,  126.4f,  37.1f,   1.8f,  38.2f);      // polka 2

    for(int n = 1; n <= 5; n++)
    {
        float h = 45.0f + 185.0 / 6 * n;
        DrawBox(213.1f, h,  51.8f,  37.1f,   1.8f,  72.8f);      // polka 2
    }*/
//     DrawBox(213.1f, 201.0f,   51.8f,  37.1f,   1.8f, 112.8f);      // polka 2
}

void Scene::DrawBox(float ox, float oy, float oz, float w, float h, float d)
{
    static const int poly[6][4] = {
        { 0, 1, 2, 3 },
        { 7, 6, 5, 4 },
        { 3, 2, 6, 7 },
        { 2, 1, 5, 6 },
        { 1, 0, 4, 5 },
        { 0, 3, 7, 4 }
    };

    static const float normal[6][3] = {
        {  0.0f, -1.0f,  0.0f },
        {  0.0f,  1.0f,  0.0f },
        {  0.0f,  0.0f,  1.0f },
        {  1.0f,  0.0f,  0.0f },
        {  0.0f,  0.0f, -1.0f },
        { -1.0f,  0.0f,  0.0f }
    };

    float vertex[8][3] = {
        { ox,      oy,      oz     },
        { ox + w,  oy,      oz     },
        { ox + w,  oy,      oz + d },
        { ox,      oy,      oz + d },
        { ox,      oy + h,  oz     },
        { ox + w,  oy + h,  oz     },
        { ox + w,  oy + h,  oz + d },
        { ox,      oy + h,  oz + d }
    };

    for(int n = 0; n < 6; n++)
    {
        glBegin(GL_POLYGON);
            glNormal3fv(normal[n]);
            glVertex3fv(vertex[poly[n][0]]);
            glVertex3fv(vertex[poly[n][1]]);
            glVertex3fv(vertex[poly[n][2]]);
            glVertex3fv(vertex[poly[n][3]]);
        glEnd();
    }
}

void Scene::DrawBox2(float ox, float oy, float oz,
                     float wl, float dl, float wh, float dh,
                     float sx, float sz, float h)
{
    static const int poly[6][4] = {
        { 0, 1, 2, 3 },
        { 7, 6, 5, 4 },
        { 3, 2, 6, 7 },
        { 2, 1, 5, 6 },
        { 1, 0, 4, 5 },
        { 0, 3, 7, 4 }
    };

    float vertex[8][3] = {
        { ox,       oy,      oz      },
        { ox + wl,  oy,      oz      },
        { ox + wl,  oy,      oz + dl },
        { ox,       oy,      oz + dl },
        { ox + sx,       oy + h,  oz + sz      },
        { ox + sx + wh,  oy + h,  oz + sz      },
        { ox + sx + wh,  oy + h,  oz + sz + dh },
        { ox + sx,       oy + h,  oz + sz + dh }
    };

    for(int n = 0; n < 6; n++)
    {
        float normal[3];

        float ax = vertex[poly[n][1]][0] - vertex[poly[n][0]][0];
        float ay = vertex[poly[n][1]][1] - vertex[poly[n][0]][1];
        float az = vertex[poly[n][1]][2] - vertex[poly[n][0]][2];

        float bx = vertex[poly[n][2]][0] - vertex[poly[n][0]][0];
        float by = vertex[poly[n][2]][1] - vertex[poly[n][0]][1];
        float bz = vertex[poly[n][2]][2] - vertex[poly[n][0]][2];

        float nx =  ay * bz - az * by;
        float ny = -ax * bz + az * bx;
        float nz =  ax * by - ay * bx;

        float invLen = 1.0f / sqrt(nx * nx + ny * ny + nz * nz);

        normal[0] = nx * invLen;
        normal[1] = ny * invLen;
        normal[2] = nz * invLen;

        glBegin(GL_POLYGON);
            glNormal3fv(normal);
            glVertex3fv(vertex[poly[n][0]]);
            glVertex3fv(vertex[poly[n][1]]);
            glVertex3fv(vertex[poly[n][2]]);
            glVertex3fv(vertex[poly[n][3]]);
        glEnd();
    }
}


class Engine
{
public:
    // constructor & destructor
    Engine(Scene* scene);
    ~Engine();

    // public methods
    static void DisplayHelper();
    static void ReshapeHelper(GLint width, GLint height);
    static void AnimateHelper();
    static void MouseButtonHelper(int button, int state, int x, int y);
    static void MouseMotionHelper(int x, int y);
    static void KeyboardHelper(unsigned char key, int x, int y);

private:
    static Engine* m_instance;
    Scene* m_scene;

    float m_position[3];
    float m_speed;
    float m_speedL;
    float m_accel;
    float m_accelL;
    float m_xRotation;
    float m_yRotation;
    int   m_width;
    int   m_height;

    bool  m_rotationFlag;
    int   m_mouseOldX;
    int   m_mouseOldY;

    int   m_programId;

    void Display();
    void Reshape(GLint width, GLint height);
    void Animate();
    void MouseButton(int button, int state, int x, int y);
    void MouseMotion(int x, int y);
    void Keyboard(unsigned char key, int x, int y);
};

Engine* Engine::m_instance = nullptr;

// constructor & destructor
Engine::Engine(Scene* scene)
{
    m_instance = this;
    m_scene = scene;

    m_position[0] = 150.0f;
    m_position[1] = 150.0f;
    m_position[2] = 300.0f;

    m_speed     = 0.0f;
    m_speedL    = 0.0f;
    m_accel     = 0.0f;
    m_accelL    = 0.0f;
    m_xRotation = 0.0f;
    m_yRotation = 0.0f;

    m_width  = 1280;
    m_height = 720;

    m_rotationFlag = false;
    m_mouseOldX    = 0;
    m_mouseOldY    = 0;

    m_programId = glCreateProgramObjectARB();
    GLenum vid  = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    GLenum fid  = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    glShaderSourceARB(vid, 1, (const GLcharARB**)&vertexShader, NULL);
    glShaderSourceARB(fid, 1, (const GLcharARB**)&fragmentShader, NULL);

    glCompileShaderARB(vid);
    glCompileShaderARB(fid);

    glAttachObjectARB(m_programId, vid);
    glAttachObjectARB(m_programId, fid);

    glLinkProgramARB(m_programId);
}

Engine::~Engine()
{
    m_instance = nullptr;
}

// public methods
void Engine::DisplayHelper()
{
    m_instance->Display();
}

void Engine::ReshapeHelper(GLint width, GLint height)
{
    m_instance->Reshape(width, height);
}

void Engine::AnimateHelper()
{
    m_instance->Animate();
}

void Engine::MouseButtonHelper(int button, int state, int x, int y)
{
    m_instance->MouseButton(button, state, x, y);
}

void Engine::MouseMotionHelper(int x, int y)
{
    m_instance->MouseMotion(x, y);
}

void Engine::KeyboardHelper(unsigned char key, int x, int y)
{
    m_instance->Keyboard(key, x, y);
}

// private methods
void Engine::Display(void)
{
    static float matrix[16];

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(m_xRotation, 1.0f, 0.0f, 0.0f);
    glRotatef(m_yRotation, 0.0f, 1.0f, 0.0f);
    glTranslatef(-m_position[0], -m_position[1], -m_position[2]);

    glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

    glUseProgramObjectARB(m_programId);
    glUniformMatrix4fv(glGetUniformLocation(m_programId, "lightModelViewMatrix"), 1, GL_FALSE, matrix);

    m_scene->Draw(m_position[1]);
    glUseProgramObjectARB(0);

    glutSwapBuffers();
}

void Engine::Reshape(GLint width, GLint height)
{
    m_width  = width;
    m_height = height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float left   = -1.0f;
    float right  =  1.0f;
    float top    = -static_cast<float>(height) / width;
    float bottom = -top;
    float near   = 1.0f;
    float far    = 100000.0f;

    glFrustum(left, right, top, bottom, near, far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Engine::Animate()
{
    Matrix viewMatrix;

    viewMatrix  = Matrix().RotY(m_yRotation * M_PI / 180.0);
    viewMatrix *= Matrix().RotX(m_xRotation * M_PI / 180.0);

    m_position[0] += viewMatrix.m[0][2] * m_speed;
    m_position[1] += viewMatrix.m[1][2] * m_speed;
    m_position[2] += viewMatrix.m[2][2] * m_speed;

    m_position[0] += viewMatrix.m[0][0] * m_speedL;
    m_position[1] += viewMatrix.m[1][0] * m_speedL;
    m_position[2] += viewMatrix.m[2][0] * m_speedL;

    m_speed  = m_speed  * 0.9 + m_accel  * 0.1;
    m_speedL = m_speedL * 0.9 + m_accelL * 0.1;

    m_accel  *= 0.7;
    m_accelL *= 0.7;

    glutPostRedisplay();
}

void Engine::MouseButton(int button, int state, int x, int y)
{
    if (button == 0)
    {
        if (state == 0)
        {
            m_rotationFlag = true;
            m_mouseOldX    = x;
            m_mouseOldY    = y;
        }
        else
        {
            m_rotationFlag = false;
        }
    }
}

void Engine::MouseMotion(int x, int y)
{
    if (m_rotationFlag)
    {
        m_yRotation += (x - m_mouseOldX) / 8.0f;
        m_xRotation += (y - m_mouseOldY) / 8.0f;

        m_mouseOldX = x;
        m_mouseOldY = y;
    }

    glutPostRedisplay();
}

void Engine::Keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27:
            exit(0);

        case 'w':
            m_accel = -2.0f;
            break;

        case 's':
            m_accel = 2.0f;
            break;

        case 'a':
            m_accelL = -2.0f;
            break;

        case 'd':
            m_accelL = 2.0f;
            break;

        break;
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(1280, 720);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("GL Test");

    Scene scene;
    Engine engine(&scene);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    glutDisplayFunc (Engine::DisplayHelper);
    glutReshapeFunc (Engine::ReshapeHelper);
    glutIdleFunc    (Engine::AnimateHelper);
    glutKeyboardFunc(Engine::KeyboardHelper);
    glutMouseFunc   (Engine::MouseButtonHelper);
    glutMotionFunc  (Engine::MouseMotionHelper);

    glutMainLoop();

    return 0;
}

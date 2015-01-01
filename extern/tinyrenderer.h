#ifndef TINY_RENDERER_H
#define TINY_RENDERER_H
#include <cmath>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

namespace TinyRenderer
{
class Renderer
{
public:
    Renderer(float inWidth, float inHeight, bool In2D = false);
    void SetBackground(const float * colour);
    void InitGL();
    void Clear();
    void DrawPoint(const float * pt, const float * colour = 0, float thickness = 3.f);
    void DrawLine(const float * start, const float *end, const float * colour = 0, float thickness = 1.f);
    void DrawTriangle(const float * a, const float * b, const float * c, const float * colour = 0, float thickness = 1.f);
    void DrawSolidTriangle(const float * a, const float * b, const float * c, const float * colour);
    void DrawCircle(const float * center, float radius, const float * normal, int numSections=16, const float * colour = 0, float thickness=1.f);
    //tris is just an index buffer, but we treat it as triangles (num indices = numTris * 3)
    void DrawMesh(const float * vertices, const unsigned short * tris, int numTris, int stride=12, const float * colour = 0, bool solid=true, float thickness =1.f);
    void DrawSphere(const float * center, float radius, int numSections=16, const float * colour = 0, float thickness=1.f);
    void SetCameraPosition(const float * eye);
    void SetCameraLookAt(const float * eye, const float * target, const float * up);
    void SetCameraLense(float angleFOV, float near=1.f, float far=1000.f);
    void SetDimensions(float width, float height);
    void Flush();
private:
    float width;
    float height;
    float mv[16];
    float projection[16];
    bool b2D;

    void DrawVertex(const float* v);
};

inline void Renderer::DrawVertex(const float* v)
{
    if(b2D == false)
    {
        glVertex3f(v[0], v[1], v[2]);
    }else
    {
        glVertex3f(v[0], v[1], 0.f);
    }
}

inline void CrossProduct(const float * a, const float * b, float * c)
{
    c[0] = a[1]*b[2] - a[2]*b[1];
    c[1] = a[2]*b[0] - a[0]*b[2];
    c[2] = a[0]*b[1] - a[1]*b[0];
}

inline float DotProduct(const float * a, const float * b)
{
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

inline void Normalize(const float * x, float * xn)
{
    float l = sqrt(DotProduct(x,x));
    xn[0] = x[0] / l;
    xn[1] = x[1] / l;
    xn[2] = x[2] / l;
}

inline float AnglesToRadians(float angle)
{
    return 3.14159 * angle / 180.f;
}

inline bool IsZero(float v)
{
    return v < 1e-4 && v > -1e-4;
}

inline Renderer::Renderer(float inWidth, float inHeight, bool bIn2D)
{
    b2D = bIn2D;
    InitGL();
    SetDimensions(inWidth, inHeight);
    
    float eye[3] = {0,0,0};
    float target[3] = {0,0,-1};
    float up[3] = {0,0,1};
    SetCameraLookAt(eye,target,up);
    SetCameraLense(90);
}

inline void Renderer::SetBackground(const float * colour)
{
    glClearColor(colour[0], colour[1], colour[2], 0.f);
}

inline void Renderer::InitGL()
{
    glShadeModel( GL_SMOOTH );
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClearDepth( 1.0f );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    
    glEnableClientState(GL_VERTEX_ARRAY);
    
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );
}

inline void Renderer::SetDimensions(float inWidth, float inHeight)
{
    width = inWidth;
    height = inHeight;
    
    glViewport(0, 0, width, height);
}

inline void Renderer::Clear()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

inline void SetColour(const float * colour)
{
    if(colour)
    {
        glColor3f(colour[0], colour[1], colour[2]);
    }else
    {
        glColor3f(1.f, 1.f, 1.f);
    }
}

inline void Renderer::DrawPoint(const float * pt, const float * colour, float thickness)
{
    SetColour(colour);   
    glPointSize(thickness);
    glBegin(GL_POINTS);
    DrawVertex(pt);
    glEnd();
    
}

inline void Renderer::DrawLine(const float * start, const float * end, const float * colour, float thickness)
{
    glLineWidth(thickness);
    SetColour(colour);
    glBegin(GL_LINES);
    DrawVertex(start);
    DrawVertex(end);
    glEnd();
}

inline void Renderer::DrawTriangle(const float * a, const float * b, const float * c, const float * colour, float thickness)
{
    DrawLine(a,b,colour,thickness);
    DrawLine(b,c,colour,thickness);
    DrawLine(c,a,colour,thickness);
}

inline void Renderer::DrawSolidTriangle(const float * a, const float * b, const float * c, const float * colour)
{
    SetColour(colour);
    glBegin(GL_TRIANGLES);
    DrawVertex(a);
    DrawVertex(b);
    DrawVertex(c);
    glEnd();
}

inline void Renderer::DrawMesh(const float * vertices, const unsigned short * tris, int numTris, int stride, const float * colour, bool solid, float thickness)
{
    SetColour(colour);
    if(solid)
    {
        glVertexPointer(3, GL_FLOAT, stride, vertices);    
        glDrawElements(GL_TRIANGLES, numTris*3, GL_UNSIGNED_SHORT, tris);
    }else
    {
        for(int i=0; i<numTris; i++)
        {
            int a = tris[i*3];
            const float * vA = &vertices[a*stride/sizeof(float)];
        
            int b = tris[i*3+1];
            const float * vB = &vertices[b*stride/sizeof(float)];
        
            int c = tris[i*3+2];
            const float * vC = &vertices[c*stride/sizeof(float)];
        
            if(solid)
            {
                DrawSolidTriangle(vA,vB,vC,colour);            
            }else
            {
                DrawTriangle(vA,vB,vC,colour,thickness);
            }

        } 
    }
}

inline void Renderer::DrawCircle(const float * center, float radius, const float * normal, int numSections, const float * colour, float thickness)
{
    SetColour(colour);
    
    float xAxis[3];
    float yAxis[3];
    
    if(IsZero(normal[0]) && IsZero(normal[1]))
    {
        xAxis[0] = 1.f;
        xAxis[1] = xAxis[2] = 0.f;
        
        yAxis[1] = 1.f;
        yAxis[0] = yAxis[2] = 0.f;
    }else
    {
        xAxis[0] = normal[1];
        xAxis[1] = -normal[0];
        xAxis[2] = 0.f;     
        CrossProduct(xAxis, normal, yAxis);
        Normalize(xAxis, xAxis);
        Normalize(yAxis, yAxis);
    }
    

    float a[3];
    float b[3];
    float rads = 2.f*3.14159/numSections;
        
    for(int j=0; j<numSections; j++)
    {
        float c = cos(rads*j);
        float s = sin(rads*j);

        a[0] = center[0] + radius*(xAxis[0] * c + yAxis[0] * s);
        a[1] = center[1] + radius*(xAxis[1] * c + yAxis[1] * s);
        a[2] = center[2] + radius*(xAxis[2] * c + yAxis[2] * s);

        float c2 = cos(rads*(j+1));
        float s2 = sin(rads*(j+1));

        b[0] = center[0] + radius*(xAxis[0] * c2 + yAxis[0] * s2);;
        b[1] = center[1] + radius*(xAxis[1] * c2 + yAxis[1] * s2);
        b[2] = center[2] + radius*(xAxis[2] * c2 + yAxis[2] * s2);
    
        DrawLine(a,b,colour,thickness);
    }
}

inline void Renderer::DrawSphere(const float * center, float radius, int numSections, const float * colour, float thickness)
{
    SetColour(colour);
    float a[3];
    float b[3];
    
    float rads = 2.f*3.14159/numSections;
    float unit = 1.f/numSections;
    
    for(int i=0; i<numSections; i++)
    {
        float offset = radius*unit*i*2 - radius;
        float r = sqrt(radius*radius - offset*offset);
        for(int j=0; j<numSections; j++)
        {
            a[0] = center[0] + r * cos(rads*j);
            a[1] = center[1] + offset;
            a[2] = center[2] + r * sin(rads*j);
        
            b[0] = center[0] + r * cos(rads*(j+1));
            b[1] = center[1] + offset;
            b[2] = center[2] + r * sin(rads*(j+1));
        
            DrawLine(a,b,colour,thickness);
        }
        
        for(int j=0; j<numSections; j++)
        {
            a[0] = center[0] + offset;
            a[1] = center[1] + r * cos(rads*j);
            a[2] = center[2] + r * sin(rads*j);
        
            b[0] = center[0] + offset;
            b[1] = center[1] + r * cos(rads*(j+1));
            b[2] = center[2] + r * sin(rads*(j+1));
        
            DrawLine(a,b,colour,thickness);
        }
        
    }
}

inline void Renderer::Flush()
{
    glFlush();
}

inline void Renderer::SetCameraPosition(const float * eye)
{
    glMatrixMode( GL_MODELVIEW );
    glLoadMatrixf(mv);
    glTranslatef(-eye[0], -eye[1], -eye[2]);    
}


inline void Renderer::SetCameraLookAt(const float * eye, const float * target, const float * refUp)
{
    float side[3];
    float forward[3];
    float up[3];
    
    forward[0] = target[0] - eye[0];
    forward[1] = target[1] - eye[1];
    forward[2] = target[2] - eye[2];
    
    Normalize(forward, forward);
    Normalize(refUp, up);

    CrossProduct(forward,up, side); //compute side
    CrossProduct(side,forward,up);  //compute orthogonal up
    
    mv[0] = side[0];
    mv[1] = up[0];
    mv[2] = -forward[0];
    mv[3] = 0.f;

    mv[4] = side[1];
    mv[5] = up[1];
    mv[6] = -forward[1];
    mv[7] = 0.f;

    mv[8] = side[2];
    mv[9] = up[2];
    mv[10] = -forward[2];
    mv[11] = 0.f;

    mv[12] = 0;
    mv[13] = 0;
    mv[14] = 0;
    mv[15] = 1.f;
    
    glMatrixMode( GL_MODELVIEW );
    glLoadMatrixf(mv);
    glTranslatef(-eye[0], -eye[1], -eye[2]);
}



inline void Renderer::SetCameraLense(float angleFOV, float near, float far)
{
    const float ratio = (width > 0.f && height > 0.f) ? width / height : 1.f;
    const float size = tanf(AnglesToRadians(angleFOV) / 2.f);
    
    float right = size;
    float top = size / ratio;
    
    projection[0] = near/right;
    projection[1] = 0.f;
    projection[2] = 0.f;
    projection[3] = 0.f;
        
    projection[4] = 0.f;
    projection[5] = near/top;
    projection[6] = 0.f;
    projection[7] = 0.f;
    
    projection[8] = 0.f;
    projection[9] = 0.f;
    projection[10] = -(far+near) / (far - near);
    projection[14] = -2.f*far*near / (far - near);
        
    projection[12] = 0.f;
    projection[13] = 0.f;
    projection[11] = -1.f;
    projection[15] = 0.f;
    
    glMatrixMode( GL_PROJECTION );
    glLoadMatrixf(projection);
}

}

#endif

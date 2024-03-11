#shader fragment
#version 330 core

in vec2 vert_pos;

layout(location = 0) out vec4 color;

uniform vec2 seed;
uniform vec4 window;
uniform int maxIter;
uniform vec3 fadeColor;

float pow8(float x)
{
    float tmp = x * x;
    x = tmp * tmp;
    return x * x;
}

float pow16(float x)
{
    float tmp = x * x;
    x = tmp * tmp;
    tmp = x * x;
    return tmp * tmp;
}

vec4 isPointInMandelbrotSet(float cx, float cy)
{
    float currentx = 0.0;
    float currenty = 0.0;

    float xTmp;

    int nbIter = 0;
    while(currentx * currentx + (currenty * currenty) < 4.0 && nbIter < maxIter)
    {
        xTmp = currentx;
        currentx = (xTmp * xTmp - (currenty * currenty)) + cx;
        currenty = 2.0 * xTmp * currenty + cy;
        nbIter++;
    }

    float fade = 1.0 - (nbIter >= maxIter ? 1.0 : float(nbIter) / float(maxIter));

    return vec4(fade * fadeColor.x, fade * fadeColor.y, fade * fadeColor.z, 1.0);
}

vec4 isPointInJuliaSet(float zx, float zy, float cx, float cy)
{
    float currentx = zx;
    float currenty = zy;

    float xTmp;

    int nbIter = 0;
    while(currentx * currentx + (currenty * currenty) < 4.0 && nbIter < maxIter)
    {
        xTmp = currentx;
        currentx = (xTmp * xTmp - (currenty * currenty)) + cx;
        currenty = 2.0 * xTmp * currenty + cy;
        nbIter++;
    }

    float fade = nbIter >= maxIter ? 0.0 : pow16(1.0 - (float(nbIter) / float(maxIter)));

    return vec4(fade * fadeColor.x, fade * fadeColor.y, fade * fadeColor.z, 1.0);
}
        
void main()
{
    float posX = (vert_pos.x + 1.0) * 0.5 * (window.y - window.x) + window.x;
    float posY = (vert_pos.y + 1.0) * 0.5 * (window.w - window.z) + window.z;

    color = isPointInJuliaSet(posX, posY, seed.x, seed.y);
    //color = isPointInMandelbrotSet(pos.x, pos.y);
};

#shader vertex        
#version 330 core
        
layout(location = 0) in vec2 position;

out vec2 vert_pos;

uniform vec2 seed;
uniform vec4 window;
uniform int maxIter;
uniform vec3 fadeColor;
        
void main()
{
    vert_pos = position;
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
};

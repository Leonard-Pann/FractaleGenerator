#shader fragment
#version 330 core

in vec2 vert_pos;

layout(location = 0) out vec4 color;

uniform vec2 seed;
uniform vec4 window;
uniform int maxIter;
uniform vec3 inColor;
uniform vec3 outColor;

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

float pow32(float x)
{
    float tmp = x * x;
    x = tmp * tmp;
    tmp = x * x;
    x = tmp * tmp;
    return x * x;
}

float pow64(float x)
{
    float tmp = x * x;
    x = tmp * tmp;
    tmp = x * x;
    x = tmp * tmp;
    tmp = x * x;
    return tmp * tmp;
}

float smoothStep(float x)
{
    return -2.0 * x * x * x + 3.0 * x * x;
}

vec3 LerpColor(vec3 startCol, vec3 endCol, float t)
{
    return (endCol.xyz - startCol.xyz) * t + startCol.xyz;
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

    if(nbIter >= maxIter)
    {
        return vec4(inColor.xyz, 1.0);
    }

    float fade = pow32(1.0 - (float(nbIter) / float(maxIter)));
    return vec4(LerpColor(inColor, outColor, fade), 1.0);
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

    if(nbIter >= maxIter)
    {
        return vec4(inColor.xyz, 1.0);
    }

    float fade = pow32(1.0 - (float(nbIter) / float(maxIter)));
    return vec4(LerpColor(inColor, outColor, fade), 1.0);
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
uniform vec3 inColor;
uniform vec3 outColor;
        
void main()
{
    vert_pos = position;
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
};

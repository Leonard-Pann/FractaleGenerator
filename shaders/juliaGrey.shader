#shader fragment
#version 310 es
precision highp float;

in vec2 texCoord;
layout(location = 0) out float grey;

uniform vec2 seed;
uniform vec4 window;
uniform int maxIter;
uniform ivec2 textureSize;

float pow64(float x)
{
    float tmp = x * x;
    x = tmp * tmp;
    tmp = x * x;
    x = tmp * tmp;
    tmp = x * x;
    return tmp * tmp;
}

float getJuliaGreyScale(float zx, float zy, float cx, float cy)
{
    float currentx = zx;
    float currenty = zy;
    float xTmp;

    int nbIter = 0;
    while (currentx * currentx + (currenty * currenty) < 4.0 && nbIter < maxIter)
    {
        xTmp = currentx;
        currentx = (xTmp * xTmp) - (currenty * currenty) + cx;
        currenty = 2.0 * xTmp * currenty + cy;
        nbIter++;
    }

    if (nbIter >= maxIter)
        return 0.0;

    float fade = pow64(1.0 - (float(nbIter) / float(maxIter)));
    return fade;
}

void main()
{
    float posX = texCoord.x * (window.y - window.x) + window.x;
    float posY = texCoord.y * (window.w - window.z) + window.z;

    grey = getJuliaGreyScale(posX, posY, seed.x, seed.y);
}

#shader vertex
#version 310 es
precision highp float;

layout(location = 0) in vec2 position;
out vec2 texCoord;

void main()
{
    texCoord = (position + 1.0) * 0.5;
    gl_Position = vec4(position, 0.0, 1.0);
}
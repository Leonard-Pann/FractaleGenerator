#shader fragment
#version 330 core

in vec2 vert_pos;

layout(location = 0) out vec4 color;

uniform vec2 seed;
uniform vec4 window;
uniform int maxIter;

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

vec4 getJuliaColor(float zx, float zy, float cx, float cy)
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

    vec3 inColor = vec3(0.0, 0.0, 0.0);
    vec3 outColor = vec3(1.0, 1.0, 1.0);

    if (nbIter >= maxIter)
    {
		return vec4(inColor.xyz, 1.0);
    }

    float fade = pow64(1.0 - (float(nbIter) / float(maxIter)));
    return vec4(mix(inColor, outColor, fade), 1.0);
}
        
void main()
{
    float posX = (vert_pos.x + 1.0) * 0.5 * (window.y - window.x) + window.x;
    float posY = (vert_pos.y + 1.0) * 0.5 * (window.w - window.z) + window.z;

    color = getJuliaColor(posX, posY, seed.x, seed.y);
};

#shader vertex        
#version 330 core
        
layout(location = 0) in vec2 position;

out vec2 vert_pos;

uniform vec2 seed;
uniform vec4 window;
uniform int maxIter;

void main()
{
    vert_pos = position;
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
};
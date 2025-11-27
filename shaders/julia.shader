#shader fragment
#version 310 es
precision highp float;

in vec2 vert_pos;
layout(location = 0) out vec4 color;

uniform vec2 seed;
uniform vec4 window;
uniform int maxIter;
uniform vec3 inColor;
uniform vec3 colorPalette[6];
uniform int nbColors;
uniform float colorRange;

float pow32(float x)
{
    float tmp = x * x;
    x = tmp * tmp;
    tmp = x * x;
    x = tmp * tmp;
    return x * x;
}

vec4 getColor(float nbIter, float maxIter)
{
    float value = nbIter / maxIter;
    vec3 col = vec3(1.0);

    float nbCols = float(nbColors) - 1.0;

    for (int i = 0; i < nbColors - 1; i++)
    {
        float minValue = float(i) / nbCols;
        float maxValue = float(i + 1) / nbCols;

        if (value >= minValue && value <= maxValue)
        {
            col = mix(colorPalette[i], colorPalette[i + 1], (value - minValue) * nbCols);
            break;
        }
    }

    return vec4(col, 1.0);
}

float length2(float x, float y)
{
    return sqrt(x * x + (y * y));
}

vec4 getJuliaColor(float zx, float zy, float cx, float cy)
{
    float currentx = zx;
    float currenty = zy;

    float smoothValue = exp(-length2(currentx, currenty));
    float colorMod = float(maxIter) * 0.01 * colorRange;

    int nbIter = 0;
    while (currentx * currentx + currenty * currenty < 4.0 && nbIter < maxIter)
    {
        float xtmp = currentx;
        currentx = (xtmp * xtmp - currenty * currenty) + cx;
        currenty = 2.0 * xtmp * currenty + cy;

        smoothValue += exp(-length2(currentx, currenty));
        nbIter++;
    }

    if (nbIter >= maxIter)
        return vec4(inColor, 1.0);

    float floorSV = floor(smoothValue);
    float smoothFrac = smoothValue - floorSV;
    float shifted = mod(floorSV, float(maxIter)) + smoothFrac;

    float hundredORange = 100.0 / colorRange;
    float iter = mod(shifted, hundredORange);

    return getColor(iter, hundredORange);
}

void main()
{
    float posX = (vert_pos.x + 1.0) * 0.5 * (window.y - window.x) + window.x;
    float posY = (vert_pos.y + 1.0) * 0.5 * (window.w - window.z) + window.z;

    color = getJuliaColor(posX, posY, seed.x, seed.y);
}

#shader vertex        
#version 310 es

layout(location = 0) in vec2 position;

out vec2 vert_pos;

void main()
{
    vert_pos = position;
    gl_Position = vec4(position, 0.0, 1.0);
}
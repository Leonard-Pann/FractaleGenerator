#shader fragment
#version 310 es
precision highp float;

in vec2 vert_pos;
layout(location = 0) out vec4 color;

#define MAX_ITER 750

uniform vec2 seed;
uniform vec4 window;
uniform vec3 inColor;
uniform vec3 colorPalette[6];
uniform int nbColors;
uniform float colorRange;

vec4 getJuliaColor(float currentx, float currenty, float cx, float cy)
{
    float xSquare = currentx * currentx;
    float ySquare = currenty * currenty;
    float smoothValue = exp(-sqrt(xSquare + ySquare));

    int nbIter;
    for(nbIter = 0; nbIter < MAX_ITER; nbIter++)
    {
        if(xSquare + ySquare >= 4.0)
        {
            break;
        }

        currenty = 2.0 * currentx * currenty + cy;
        currentx = xSquare - ySquare + cx;
        xSquare = currentx * currentx;
        ySquare = currenty * currenty;
        smoothValue += exp(-sqrt(xSquare + ySquare));
    }

    if (nbIter >= MAX_ITER)
    {
        return vec4(inColor.xyz, 1.0);
    }

    // other smoothing function : 
    // float log_zn = log(xSquare + ySquare) * 0.5;
    // float oneOlog2 = 1.0 / log(2.0);
    // float nu = log(log_zn * oneOlog2) * oneOlog2;
    // float smoothIter = float(nbIter) + 1.0 - nu;
    // float normalized = smoothIter / float(MAX_ITER);
    // normalized = mod(normalized * colorRange, 1.0);
    // float end = float(nbColors - 1);
    // float scaled = normalized * end;
    // int i = int(floor(scaled));
    // float t = scaled - float(i);
    // vec3 c0 = colorPalette[i];
    // vec3 c1 = colorPalette[min(i + 1, nbColors - 1)];
    // return vec4(mix(c0, c1, t), 1.0);



    float floorSV = floor(smoothValue);
    float smoothFrac = smoothValue - floorSV;
    float shifted = mod(floorSV, float(maxIter)) + smoothFrac;

    float hundredORange = 100.0 / colorRange;
    float iter = mod(shifted, hundredORange);

    float value = iter / hundredORange;
    float end = float(nbColors - 1);
    float valueTEnd = value * end;
    int i = int(floor(valueTEnd));
    vec3 color = mix(colorPalette[i], colorPalette[i + 1], valueTEnd - float(i));
    return vec4(color.xyz, 1.0);
}

void main()
{
    float posX = 0.5 * (vert_pos.x + 1.0) * (window.y - window.x) + window.x;
    float posY = 0.5 * (vert_pos.y + 1.0) * (window.w - window.z) + window.z;

    color = getJuliaColor(posX, posY, seed.x, seed.y);
}

#shader vertex        
#version 310 es
precision highp float;

layout(location = 0) in vec2 position;
out vec2 vert_pos;

void main()
{
    vert_pos = position;
    gl_Position = vec4(position, 0.0, 1.0);
}

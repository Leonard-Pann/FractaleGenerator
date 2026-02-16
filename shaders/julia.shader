#shader fragment
#version 460 core

in vec2 vert_pos;
layout(location = 0) out vec4 color;

#define MAX_ITER 650

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
    int nbIter;

    for(nbIter = 0; nbIter < MAX_ITER; nbIter++)
    {
        if(xSquare + ySquare >= 200.0)
        {
            break;
        }

        currenty = 2.0 * currentx * currenty + cy;
        currentx = xSquare - ySquare + cx;
        xSquare = currentx * currentx;
        ySquare = currenty * currenty;
    }

    if (nbIter >= MAX_ITER)
    {
        return vec4(inColor.xyz, 1.0);
    }

    float smoothIter = float(nbIter) + 2.52876637294 - log2(log2(xSquare + ySquare)); // 2.52876637294 = 2 - log2(ln(2.0))
    float t = fract(smoothIter / 100.0);
    float colF = float(nbColors - 1);
    float tCol = t * colF;
    int index = int(floor(tCol));

    float t2 = t * colF - float(index);
    vec3 color = mix(colorPalette[index], colorPalette[index + 1], t2);
    return vec4(color.xyz, 1.0);

    // float maxModSquare = 16.0 + (cx * cx) + (cy * cy) + (8.0 * (abs(cx) + abs(cy)));
    // float t = (xSquare + ySquare) / maxModSquare;

    // float sn = float(nbIter) + 0.75;
    // float logNorm = log2(xSquare + ySquare);
    // if(logNorm > 0.0)
    // {
    //     sn = sn - log2(logNorm);
    // }
    // else
    // {
    //     sn = sn - logNorm;
    // }

    // float value = mod(sn * colorRange, float(nbColors - 1));
    // float fv = floor(value);
    // float fraction = value - fv;
    // int i = int(fv);
    // vec3 color = mix(colorPalette[i], colorPalette[i + 1], fraction);
    // return vec4(color.xyz, 1.0);



    // float floorSV = floor(smoothValue);
    // float smoothFrac = smoothValue - floorSV;
    // float shifted = mod(floorSV, float(MAX_ITER)) + smoothFrac;

    // float hundredORange = 100.0 / colorRange;
    // float iter = mod(shifted, hundredORange);

    // float value = iter / hundredORange;
    // float end = float(nbColors - 1);
    // float valueTEnd = value * end;
    // int i = int(floor(valueTEnd));
    // vec3 color = mix(colorPalette[i], colorPalette[i + 1], valueTEnd - float(i));
    // return vec4(color.xyz, 1.0);
}

void main()
{
    float posX = 0.5 * (vert_pos.x + 1.0) * (window.y - window.x) + window.x;
    float posY = 0.5 * (vert_pos.y + 1.0) * (window.w - window.z) + window.z;

    color = getJuliaColor(posX, posY, seed.x, seed.y);
}

#shader vertex        
#version 460 core

layout(location = 0) in vec2 position;
out vec2 vert_pos;

void main()
{
    vert_pos = position;
    gl_Position = vec4(position, 0.0, 1.0);
}
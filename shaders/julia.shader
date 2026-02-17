#shader fragment
#version 310 es
precision highp float;

in vec2 vert_pos;
layout(location = 0) out vec4 color;

#define MAX_ITER 650

uniform vec2 seed;
uniform vec4 window;
uniform vec3 inColor;
uniform vec3 colorPalette[6];
uniform float colorRange;

vec4 getJuliaColor(float currentx, float currenty)
{
    float xSquare = currentx * currentx;
    float ySquare = currenty * currenty;
    int nbIter;

    for(nbIter = 0; nbIter < MAX_ITER; nbIter++)
    {
        if(xSquare + ySquare >= 32.0)
        {
            break;
        }

        currenty = 2.0 * currentx * currenty + seed.y;
        currentx = xSquare - ySquare + seed.x;
        xSquare = currentx * currentx;
        ySquare = currenty * currenty;
    }

    if (nbIter >= MAX_ITER)
    {
        return vec4(inColor.xyz, 1.0);
    }

    float smoothIter = float(nbIter) + 2.52876637294 - log2(log2(xSquare + ySquare)); // 2.52876637294 = 2 - log2(ln(2.0))
    xSquare = fract(smoothIter * colorRange);
    ySquare = xSquare * 5.0;
    int index = int(floor(ySquare));

    smoothIter = ySquare - float(index);
    vec3 color = mix(colorPalette[index], colorPalette[index + 1], smoothIter);
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

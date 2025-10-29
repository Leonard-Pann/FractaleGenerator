#shader fragment
#version 320 es
precision highp float;

in vec2 v_uv;
layout(location = 0) out vec4 outColor;

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

    while (currentx * currentx + currenty * currenty < 4.0 && nbIter < maxIter)
    {
        xTmp = currentx;
        currentx = (xTmp * xTmp - (currenty * currenty)) + cx;
        currenty = 2.0 * xTmp * currenty + cy;
        nbIter++;
    }

    if (nbIter >= maxIter)
        return 0.0;

    float fade = pow64(1.0 - (float(nbIter) / float(maxIter)));
    return mix(0.0, 1.0, fade);
}

void main()
{
    // Convert fragment UV to world position
    float posX = v_uv.x * (window.y - window.x) + window.x;
    float posY = v_uv.y * (window.w - window.z) + window.z;

    float greyScale = getJuliaGreyScale(posX, posY, seed.x, seed.y);

    // Output grayscale in red channel
    outColor = vec4(greyScale, greyScale, greyScale, 1.0);
}

#shader vertex
#version 320 es
precision highp float;

layout(location = 0) in vec2 position;
out vec2 v_uv;

void main()
{
    v_uv = (position + 1.0) * 0.5; // map [-1,1] to [0,1]
    gl_Position = vec4(position, 0.0, 1.0);
}

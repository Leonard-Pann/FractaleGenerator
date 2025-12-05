#version 460

layout (local_size_x = 16, local_size_y = 16) in;

uniform vec2 seed;
uniform vec4 window;
uniform int maxIter;
uniform ivec2 textureSize;

layout(std430, binding = 0) buffer Output
{
    float pixels[];
};

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
    float currentModule = fma(currentx, currentx, currenty * currenty);
    while(currentModule < 4.0 && nbIter < maxIter)
    {
        xTmp = currentx;
        currentx = fma(xTmp, xTmp, -currenty * currenty) + cx;
        currenty = fma(2.0 * xTmp, currenty, cy);
        nbIter++;
        currentModule = fma(currentx, currentx, currenty * currenty);
    }

    if (nbIter >= maxIter)
    {
        return 0.0;
    }

    float fade = pow64(1.0 - (float(nbIter) / float(maxIter)));
    return mix(0.0, 1.0, fade);
}

void main()
{
    //x, y is position in texture coordonate
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;

    if (x >= uint(textureSize.x) || y >= uint(textureSize.y))
    {
        return;
    }

    // posX, posY is from window coordonate
    float posX = fma(float(x) / float(textureSize.x - 1), window.y - window.x, window.x);
    float posY = fma(float(y) / float(textureSize.y - 1), window.w - window.z, window.z);

    float greyScale = getJuliaGreyScale(posX, posY, seed.x, seed.y);
    int index = int(fma(y, textureSize.x, x));
    pixels[index] = greyScale;
}
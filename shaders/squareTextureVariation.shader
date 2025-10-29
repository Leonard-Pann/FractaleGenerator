#shader fragment
#version 320 es
precision highp float;

in vec2 v_uv;
layout(location = 0) out vec4 outColor;

uniform sampler2D inputTex;
uniform int width;
uniform int height;

float getDifferences(vec2 texCoord)
{
    float texelX = 1.0 / float(width);
    float texelY = 1.0 / float(height);

    float sum = 0.0;
    float current = texture(inputTex, texCoord).r;

    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            if (dx == 0 && dy == 0) continue;
            vec2 offset = vec2(float(dx) * texelX, float(dy) * texelY);
            float neighbor = texture(inputTex, texCoord + offset).r;
            sum += abs(current - neighbor);
        }
    }

    return sum;
}

void main()
{
    float variation = getDifferences(v_uv) / 8.0;
    outColor = vec4(variation, 0.0, 0.0, 1.0);
}

#shader vertex
#version 320 es
precision highp float;

layout(location = 0) in vec2 position;
out vec2 v_uv;

void main()
{
    v_uv = (position + 1.0) * 0.5; // [-1,1] → [0,1]
    gl_Position = vec4(position, 0.0, 1.0);
}

#shader fragment
#version 460 core

in vec2 vert_pos;

layout(location = 0) out vec4 color;

uniform vec2 seed;
uniform vec4 window;
uniform int maxIter;
uniform vec3 inColor;
uniform vec3 colorPalette[6];
uniform int nbColors;
uniform float colorRange;

float length(float x, float y)
{
	return sqrt(x * x + (y * y));
}

vec4 getColor(float nbIter, float maxIter)
{
	float value = nbIter / maxIter;
	vec3 color;

	float minValue;
	float maxValue;
	int end = nbColors - 1;
	float nbCols = float(end);

	for (int i = 0; i < end; i++)
	{
		minValue = float(i) / nbCols;
		maxValue = float(i + 1) / nbCols;

		if (value >= minValue && value <= maxValue)
		{
			color = mix(colorPalette[i], colorPalette[i + 1], (value - minValue) * nbCols);
			break;
		}
	}

	return vec4(color.xyz, 1.0);
}

vec4 getJuliaColor(float zx, float zy, float cx, float cy)
{
	float currentx = zx;
	float currenty = zy;

	float xTmp;

	float smoothValue = exp(-length(currentx, currenty));
	float colorMod = float(maxIter) * 0.01 * colorRange;

	int nbIter = 0;
	float currentModule = fma(currentx, currentx, currenty * currenty);
	while(currentModule < 4.0 && nbIter < maxIter)
	{
		xTmp = currentx;
		// currentx = (xTmp * xTmp - (currenty * currenty)) + cx;
		// currenty = 2.0 * xTmp * currenty + cy;

		currentx = fma(xTmp, xTmp, -currenty * currenty) + cx;
		currenty = fma(2.0 * xTmp, currenty, cy);
		nbIter++;
		smoothValue += exp(-length(currentx, currenty));
		currentModule = fma(currentx, currentx, currenty * currenty);
	}

	if (nbIter >= maxIter)
	{
		return vec4(inColor.xyz, 1.0);
	}

	float floorSV = floor(smoothValue);
	float smotthValueDecimal = smoothValue - floorSV;
	float shiftedSmoothValue = int(floorSV) % maxIter + smotthValueDecimal;

	float hundredORange = 100.0 / colorRange;
	float iter = mod(float(shiftedSmoothValue), hundredORange);
	return getColor(iter, hundredORange);
}
	 
void main()
{
	// float posX = (vert_pos.x + 1.0) * 0.5 * (window.y - window.x) + window.x;
	// float posY = (vert_pos.y + 1.0) * 0.5 * (window.w - window.z) + window.z;

	float posX = fma(0.5 * (vert_pos.x + 1.0), window.y - window.x, window.x);
	float posY = fma(0.5 * (vert_pos.y + 1.0),  window.w - window.z, window.z);

	color = getJuliaColor(posX, posY, seed.x, seed.y);
};

#shader vertex        
#version 460 core
		
layout(location = 0) in vec2 position;

out vec2 vert_pos;

void main()
{
	vert_pos = position;
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
};

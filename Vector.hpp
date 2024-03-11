#ifndef __VECTOR_HPP
#define __VECTOR_HPP

struct Vector2
{
    float x, y;

    Vector2()
    {
        x = 0.0f;
        y = 0.0f;
    }

    Vector2(float x, float y)
    {
        this->x = x;
        this->y = y;
    }
};

struct Vector3
{
    float x, y, z;

    Vector3()
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }

    Vector3(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

struct Vector4
{
    float x, y, z, w;

    Vector4()
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
        w = 0.0f;
    }

    Vector4(float x, float y, float z, float w)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
};

#endif
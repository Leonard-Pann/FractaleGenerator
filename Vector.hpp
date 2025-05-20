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

    Vector2(const Vector2& vector)
    {
        this->x = vector.x;
        this->y = vector.y;
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

    Vector3(const Vector3& vector)
    {
        this->x = vector.x;
        this->y = vector.y;
        this->z = vector.z;
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

    Vector4(const Vector4& vector)
    {
        this->x = vector.x;
        this->y = vector.y;
        this->z = vector.z;
        this->w = vector.w;
    }
};

#endif
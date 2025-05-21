#ifndef __VECTOR_HPP
#define __VECTOR_HPP

struct Vector2
{
    float x, y;

    Vector2() : x(0.0f), y(0.0f)
    {

    }

    Vector2(float x, float y) : x(x), y(y)
    {

    }

    Vector2(const Vector2& vector) : x(vector.x), y(vector.y)
    {
;
    }
};

struct Vector3
{
    float x, y, z;

    Vector3() : x(0.0f), y(0.0f), z(0.0f)
    {

    }

    Vector3(float x, float y, float z) : x(x), y(y), z(z)
    {

    }

    Vector3(const Vector3& vector) : x(vector.x), y(vector.y), z(vector.z)
    {

    }
};

struct Vector4
{
    float x, y, z, w;

    Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0)
    {

    }

    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w)
    {

    }

    Vector4(const Vector4& vector) : x(vector.x), y(vector.y), z(vector.z), w(vector.w)
    {

    }
};

#endif
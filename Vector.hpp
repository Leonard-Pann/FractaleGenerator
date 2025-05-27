#ifndef __VECTOR_HPP
#define __VECTOR_HPP

#include <math.h>

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

    static float distance(const Vector2& v1, const Vector2& v2)
    {
        float tmpX = v1.x - v2.x;
        float tmpY = v1.y - v2.y;
        return sqrtf(tmpX * tmpX + (tmpY * tmpY));
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

    static float distance(const Vector3& v1, const Vector3& v2)
    {
        float tmpX = v1.x - v2.x;
        float tmpY = v1.y - v2.y;
        float tmpZ = v1.z - v2.z;
        return sqrtf(tmpX * tmpX + (tmpY * tmpY) + (tmpZ * tmpZ));
    }
};

struct Vector4
{
    float x, y, z, w;

    Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
    {

    }

    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w)
    {

    }

    Vector4(const Vector4& vector) : x(vector.x), y(vector.y), z(vector.z), w(vector.w)
    {

    }

    static float distance(const Vector4& v1, const Vector4& v2)
    {
        float tmpX = v1.x - v2.x;
        float tmpY = v1.y - v2.y;
        float tmpZ = v1.z - v2.z;
        float tmpW = v1.w - v2.w;
        return sqrtf(tmpX * tmpX + (tmpY * tmpY) + (tmpZ * tmpZ) + (tmpW * tmpW));
    }
};

struct Vector2i
{
    int x, y;

    Vector2i() : x(0), y(0)
    {

    }

    Vector2i(int x, int y) : x(x), y(y)
    {

    }

    Vector2i(const Vector2i& vector) : x(vector.x), y(vector.y)
    {
        ;
    }
};

struct Vector3i
{
    int x, y, z;

    Vector3i() : x(0), y(0), z(0)
    {

    }

    Vector3i(int x, int y, int z) : x(x), y(y), z(z)
    {

    }

    Vector3i(const Vector3i& vector) : x(vector.x), y(vector.y), z(vector.z)
    {

    }
};

struct Vector4i
{
    int x, y, z, w;

    Vector4i() : x(0), y(0), z(0), w(0)
    {

    }

    Vector4i(int x, int y, int z, int w) : x(x), y(y), z(z), w(w)
    {

    }

    Vector4i(const Vector4i& vector) : x(vector.x), y(vector.y), z(vector.z), w(vector.w)
    {

    }
};

#endif
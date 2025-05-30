#ifndef VECTOR_HPP
#define VECTOR_HPP

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

    static float sqrDistance(const Vector2& v1, const Vector2& v2)
    {
        float tmpX = v1.x - v2.x;
        float tmpY = v1.y - v2.y;
        return tmpX * tmpX + (tmpY * tmpY);
    }
};

static Vector2 operator*(const Vector2& vector, float other)
{
    return Vector2(vector.x * other, vector.y * other);
}

static Vector2 operator*(float other, const Vector2& vector)
{
    return Vector2(vector.x * other, vector.y * other);
}

static Vector2 operator+(const Vector2& left, const Vector2& right)
{
    return Vector2(left.x + right.x, left.y + right.y);
}

static Vector2 operator-(const Vector2& left, const Vector2& right)
{
    return Vector2(left.x - right.x, left.y - right.y);
}

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

    static float sqrDistance(const Vector3& v1, const Vector3& v2)
    {
        float tmpX = v1.x - v2.x;
        float tmpY = v1.y - v2.y;
        float tmpZ = v1.z - v2.z;
        return tmpX * tmpX + (tmpY * tmpY) + (tmpZ * tmpZ);
    }
};

static Vector3 operator*(const Vector3& vector, float other)
{
    return Vector3(vector.x * other, vector.y * other, vector.z * other);
}

static Vector3 operator*(float other, const Vector3& vector)
{
    return Vector3(vector.x * other, vector.y * other, vector.z * other);
}

static Vector3 operator+(const Vector3& left, const Vector3& right)
{
    return Vector3(left.x + right.x, left.y + right.y, left.z + right.z);
}

static Vector3 operator-(const Vector3& left, const Vector3& right)
{
    return Vector3(left.x - right.x, left.y - right.y, left.z - right.z);
}

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

    static float sqrDistance(const Vector4& v1, const Vector4& v2)
    {
        float tmpX = v1.x - v2.x;
        float tmpY = v1.y - v2.y;
        float tmpZ = v1.z - v2.z;
        float tmpW = v1.w - v2.w;
        return tmpX * tmpX + (tmpY * tmpY) + (tmpZ * tmpZ) + (tmpW * tmpW);
    }
};

static Vector4 operator*(const Vector4& vector, float other)
{
    return Vector4(vector.x * other, vector.y * other, vector.z * other, vector.z * other);
}

static Vector4 operator*(float other, const Vector4& vector)
{
    return Vector4(vector.x * other, vector.y * other, vector.z * other, vector.z * other);
}

static Vector4 operator+(const Vector4& left, const Vector4& right)
{
    return Vector4(left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w);
}

static Vector4 operator-(const Vector4& left, const Vector4& right)
{
    return Vector4(left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w);
}

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
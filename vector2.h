#ifndef _VECTOR2_H_
#define _VECTOR2_H_

#include <cmath>  // 用来算平方根的库（比如 √x）

// 这是一个“二维向量”的类，表示一个点或者一个方向（有x和y两个值）
class Vector2
{
public:
    double x = 0;  // 横坐标（或者x方向的值）
    double y = 0;  // 纵坐标（或者y方向的值）

public:
    Vector2() = default;    // 默认构造函数（什么都不写时x和y就是0）
    ~Vector2() = default;   // 析构函数（结束时自动处理，不用我们写）

    // 构造函数，可以直接给x和y传值，比如 Vector2(3, 4)
    Vector2(double x, double y)
        : x(x), y(y) {
    }

    // 两个向量相加，比如 (1,2) + (3,4) = (4,6)
    Vector2 operator+(const Vector2& vec) const
    {
        return Vector2(x + vec.x, y + vec.y);
    }

    // 把另一个向量加到自己身上（会改变自己）
    void operator+=(const Vector2& vec)
    {
        x += vec.x;
        y += vec.y;
    }

    // 两个向量相减，比如 (5,6) - (2,3) = (3,3)
    Vector2 operator-(const Vector2& vec) const
    {
        return Vector2(x - vec.x, y - vec.y);
    }

    // 把另一个向量减掉（会改变自己）
    void operator-=(const Vector2& vec)
    {
        x -= vec.x;
        y -= vec.y;
    }

    // 点乘（用来算夹角关系、投影），比如 (1,2)*(3,4) = 1×3 + 2×4 = 11
    double operator*(const Vector2& vec) const
    {
        return x * vec.x + y * vec.y;
    }

    // 向量乘一个数，比如 (1,2)*3 = (3,6)
    Vector2 operator*(double val) const
    {
        return Vector2(x * val, y * val);
    }

    // 自己乘一个数（会改变自己），比如 (1,2)*3 变成 (3,6)
    void operator*=(double val)
    {
        x *= val;
        y *= val;
    }

    // 判断两个向量是否完全一样（x和y都相等）
    bool operator==(const Vector2& vec) const
    {
        return x == vec.x && y == vec.y;
    }

    // 比较谁“更长”，长度大的就是更大
    bool operator>(const Vector2& vec) const
    {
        return length() > vec.length();
    }

    // 比较谁“更短”
    bool operator<(const Vector2& vec) const
    {
        return length() < vec.length();
    }

    // 计算向量的“长度”，就是 √(x² + y²)
    double length() const
    {
        return sqrt(x * x + y * y);
    }

    // 把这个向量变成“长度为1”的方向向量（不会改原来的值）
    Vector2 normalize() const
    {
        double len = length();

        if (len == 0)
            return Vector2(0, 0);  // 如果是(0,0)，就直接返回(0,0)，避免除0

        return Vector2(x / len, y / len);  // x和y都除以长度，就变成长度1
    }

    // 判断这个向量是不是“接近0”，比如长度小于0.0001
    bool approx_zero() const
    {
        return length() < 0.0001;
    }
};

#endif // !_VECTOR2_H_

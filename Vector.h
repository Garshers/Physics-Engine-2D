#pragma once

#include <math.h>

struct FlatVector {

public:
	float x, y;

	FlatVector() : x(0.0f), y(0.0f) {}

	FlatVector(float x, float y): x(x), y(y){}

    static FlatVector VecSquared(FlatVector vec) {
        return FlatVector(vec.x * vec.x, vec.y * vec.y);
    }
	static float VecLen(FlatVector vec) {
		return sqrt(vec.x * vec.x + vec.y * vec.y);
	} 

	static float Distance(const FlatVector& vec1, const FlatVector& vec2) {
		return VecLen(FlatVector(vec1.x - vec2.x, vec1.y - vec2.y));
	}
    static float DistanceSquared(const FlatVector& distanceVector) {
        return (distanceVector.x * distanceVector.x) + (distanceVector.y * distanceVector.y);
    }
    static float DistanceSquared(const FlatVector& vec1, const FlatVector& vec2) {
        float deltaX = vec2.x - vec1.x;
        float deltaY = vec2.y - vec1.y;
        return (deltaX * deltaX) + (deltaY * deltaY);
    }
	static void DivideVector(FlatVector& vec, float b) {
		vec.x /= b;
		vec.y /= b;
	}

	static void NormalizedVector(FlatVector& vec) {
		DivideVector(vec, VecLen(vec));
	}

    static float Dot(const FlatVector& a, const FlatVector& b) {
        return a.x * b.x + a.y * b.y;
    }

    static bool NearlyEqual(float a, float b) {
        return fabs(a - b) > -0.001 && fabs(a - b) < 0.001;
    }
    static bool NearlyEqual(FlatVector a, FlatVector b) {
        return NearlyEqual(a.x,b.x) && NearlyEqual(a.y, b.y);
    }

    FlatVector operator+(const FlatVector& other) const {
        return FlatVector(x + other.x, y + other.y);
    }

    FlatVector operator+(float other) const {
        return FlatVector(x + other, y + other);
    }

    FlatVector& operator+=(const FlatVector& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }
    FlatVector& operator-=(const FlatVector& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    FlatVector operator-(const FlatVector& other) const {
        return FlatVector(x - other.x, y - other.y);
    }

    FlatVector operator-() const {
        return FlatVector(-x, -y);
    }

    FlatVector operator*(float s) const {
        return FlatVector(x * s, y * s);
    }

    friend FlatVector operator*(float s, const FlatVector& v) {
        return FlatVector(v.x * s, v.y * s);
    }

    FlatVector operator/(float s) const {
        return FlatVector(x / s, y / s);
    }

    bool operator==(const FlatVector& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const FlatVector& other) const {
        return !(*this == other);
    }

    friend std::ostream& operator<<(std::ostream& os, const FlatVector& v) {
        os << "X: " << v.x << ", Y: " << v.y;
        return os;
    }
};
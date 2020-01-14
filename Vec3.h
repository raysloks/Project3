#pragma once

class Vec2;

class Vec3
{
public:
	float x, y, z;

	Vec3(void);
	Vec3(float xyz);
	Vec3(float x, float y);
	Vec3(const Vec2 & xy);
	Vec3(float x, float y, float z);
	~Vec3(void);
	
	float Len(void) const;
	float LenPwr(void) const;
	
	Vec3 & Normalize(void);
	Vec3 Normalized(void) const;

	float Dot(const Vec3 & rhs) const;

	Vec3 Cross(const Vec3 & rhs) const;
	
	const Vec3 & operator+=(const Vec3 & rhs);
	const Vec3 & operator-=(const Vec3 & rhs);
	const Vec3 & operator*=(const Vec3 & rhs);
	const Vec3 & operator/=(const Vec3 & rhs);
	const Vec3 & operator*=(float rhs);
	const Vec3 & operator/=(float rhs);
	Vec3 operator+(const Vec3 & rhs) const;
	Vec3 operator-(const Vec3 & rhs) const;
	Vec3 operator*(const Vec3 & rhs) const;
	Vec3 operator/(const Vec3 & rhs) const;
	Vec3 operator*(float rhs) const;
	Vec3 operator/(float rhs) const;

	bool operator!=(const Vec3 & rhs) const;
	bool operator==(const Vec3 & rhs) const;
	bool operator<(const Vec3 & rhs) const;
	bool operator<=(const Vec3 & rhs) const;
	bool operator>(const Vec3 & rhs) const;
	bool operator>=(const Vec3 & rhs) const;
};

Vec3 operator-(const Vec3 & rhs);
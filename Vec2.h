#pragma once

class Vec2
{
public:
	float x, y;

	Vec2(void);
	Vec2(float a);
	Vec2(float a, float b);
	~Vec2(void);
	
	float Len(void)const;
	float LenPwr(void)const;

	Vec2& Normalize(void);
	Vec2 Normalized(void) const;

	float Dot(const Vec2& rhs)const;

	void Truncate(float lim);
	
	const Vec2& operator+=(const Vec2& rhs);
	const Vec2& operator-=(const Vec2& rhs);
	const Vec2& operator*=(const Vec2& rhs);
	const Vec2& operator*=(float rhs);
	const Vec2& operator/=(float rhs);
	Vec2 operator+(const Vec2& rhs)const;
	Vec2 operator-(const Vec2& rhs)const;
	Vec2 operator*(const Vec2& rhs)const;
	Vec2 operator*(float rhs)const;
	Vec2 operator/(float rhs)const;

	bool operator!=(const Vec2& rhs)const;
	bool operator==(const Vec2& rhs)const;
	bool operator<(const Vec2& rhs)const;
	bool operator<=(const Vec2& rhs)const;
	bool operator>(const Vec2& rhs)const;
	bool operator>=(const Vec2& rhs)const;
};

Vec2 operator-(const Vec2& rhs);
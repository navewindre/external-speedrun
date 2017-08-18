#pragma once
#include <algorithm>

class vec3_t {
public:
	float x, y, z;

	vec3_t& operator+=( const vec3_t& v ) {
		x += v.x; y += v.y; z += v.z; return *this;
	}

	vec3_t& operator-=( const vec3_t& v ) {
		x -= v.x; y -= v.y; z -= v.z; return *this;
	}

	vec3_t& operator*=( float v ) {
		x *= v; y *= v; z *= v; return *this;
	}

	vec3_t operator-( const vec3_t& v ) {
		return vec3_t{ x - v.x, y - v.y, z - v.z };
	}

	vec3_t operator*( float v ) const {
		return vec3_t{ x * v, y * v, z * v };
	}

public:
	vec3_t& clamp( ) {
		x = std::clamp( x, -89.f, 89.f );
		y = std::clamp( std::remainderf( y, 360.f ), -180.f, 180.f );
		z = 0.f;

		return *this;
	}
};

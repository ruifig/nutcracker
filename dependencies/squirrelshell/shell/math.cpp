// Squirrel Shell
// Copyright (c) 2006-2008, Constantin Makshin
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "common.h"
#include <math.h>

#define DR_COEF 0.017453292519943295769236907684886f // pi / 180
#define RD_COEF 57.295779513082320876798154814105f	 // 180 / pi
#define MATH_FUNC(func) \
	SQFloat x; \
	sq_getfloat(sqvm, 2, &x); \
	sq_pushfloat(sqvm, func(x)); \
	return 1;

inline float deg2rad (float x)
{
	return (x * DR_COEF);
}

inline float rad2deg (float x)
{
	return (x * RD_COEF);
}

inline float rsqrt (float x)
{
	return (1.0f / sqrtf(x));
}

// float deg2rad(float x)
// Convert 'x' from degrees to radians
static SQInteger Deg2Rad (HSQUIRRELVM)
{
	MATH_FUNC(deg2rad);
}

// float rad2deg(float x)
// Convert 'x' from radians to degrees
static SQInteger Rad2Deg (HSQUIRRELVM)
{
	MATH_FUNC(rad2deg);
}

// float sin(float x)
// Calculate sine of 'x' (in radians)
static SQInteger Sin (HSQUIRRELVM)
{
	MATH_FUNC(sinf);
}

// float sinh(float x)
// Calculate hyperbolic sine of 'x' (in radians)
static SQInteger SinH (HSQUIRRELVM)
{
	MATH_FUNC(sinhf);
}

// float cos(float x)
// Calculate cosine of 'x' (in radians)
static SQInteger Cos (HSQUIRRELVM)
{
	MATH_FUNC(cosf);
}

// float cosh(float x)
// Calculate hyperbolic cosine of 'x' (in radians)
static SQInteger CosH (HSQUIRRELVM)
{
	MATH_FUNC(coshf);
}

// float tan(float x)
// Calculate tangent of 'x' (in radians)
static SQInteger Tan (HSQUIRRELVM)
{
	MATH_FUNC(tanf);
}

// float tanh(float x)
// Calculate hyperbolic tangent of 'x' (in radians)
static SQInteger TanH (HSQUIRRELVM)
{
#if defined(_MSC_VER) && defined(SHELL_X64)
	SQFloat x;
	sq_getfloat(sqvm, 2, &x);
	sq_pushfloat(sqvm, sinhf(x) / coshf(x));
	return 1;
#else
	MATH_FUNC(tanhf);
#endif
}

// float asin(float x)
// Calculate arcsine (in radians) of 'x'
static SQInteger ASin (HSQUIRRELVM)
{
	MATH_FUNC(asinf);
}

// float acos(float x)
// Calculate arccosine (in radians) of 'x'
static SQInteger ACos (HSQUIRRELVM)
{
	MATH_FUNC(acosf);
}

// float atan(float x)
// Calculate arctangent (in radians) of 'x'
static SQInteger ATan (HSQUIRRELVM)
{
	MATH_FUNC(atanf);
}

// float floor(float x)
// Find largest integer less than or equal to 'x'
static SQInteger Floor (HSQUIRRELVM)
{
	MATH_FUNC(floorf);
}

// float ceil(float x)
// Find smallest integer greater than or equal to 'x'
static SQInteger Ceil (HSQUIRRELVM)
{
	MATH_FUNC(ceilf);
}

// float pow(float x, float y)
// Calculate 'x' raised to power 'y'
static SQInteger Pow (HSQUIRRELVM)
{
	SQFloat x, y;
	sq_getfloat(sqvm, 2, &x);
	sq_getfloat(sqvm, 3, &y);
	sq_pushfloat(sqvm, powf(x, y));
	return 1;
}

// float exp(float x)
// Calculate e raised to power 'x'
static SQInteger Exp (HSQUIRRELVM)
{
	MATH_FUNC(expf);
}

// float sqrt(float x)
// Calculate square root of 'x'
static SQInteger Sqrt (HSQUIRRELVM)
{
	MATH_FUNC(sqrtf);
}

// float rsqrt(float x)
// Calculate reciprocal square root of 'x'
static SQInteger RSqrt (HSQUIRRELVM)
{
	MATH_FUNC(rsqrt);
}

// float log(float x)
// Calculate natural logarithm of 'x'
static SQInteger Log (HSQUIRRELVM)
{
	MATH_FUNC(logf);
}

// float log10(float x)
// Calculate base-10 logarithm of 'x'
static SQInteger Log10 (HSQUIRRELVM)
{
	MATH_FUNC(log10f);
}

void Init_Math ()
{
	SetSqFloat("PI", 3.1415926535897932384626433832795f, SQTrue);
	SetSqFloat("E", 2.7182818284590452353602874713527f, SQTrue);
	SetSqFloat("RADIANS_IN_DEGREE", DR_COEF, SQTrue);
	SetSqFloat("DEGREES_IN_RADIAN", RD_COEF, SQTrue);

	SetSqFunction("deg2rad", Deg2Rad, 2, ". f");
	SetSqFunction("rad2deg", Rad2Deg, 2, ". f");
	SetSqFunction("sin", Sin, 2, ". f");
	SetSqFunction("sinh", SinH, 2, ". f");
	SetSqFunction("cos", Cos, 2, ". f");
	SetSqFunction("cosh", CosH, 2, ". f");
	SetSqFunction("tan", Tan, 2, ". f");
	SetSqFunction("tanh", TanH, 2, ". f");
	SetSqFunction("asin", ASin, 2, ". f");
	SetSqFunction("acos", ACos, 2, ". f");
	SetSqFunction("atan", ATan, 2, ". f");
	SetSqFunction("floor", Floor, 2, ". f");
	SetSqFunction("ceil", Ceil, 2, ". f");
	SetSqFunction("pow", Pow, 3, ". f f");
	SetSqFunction("exp", Exp, 2, ". f");
	SetSqFunction("sqrt", Sqrt, 2, ". f");
	SetSqFunction("rsqrt", RSqrt, 2, ". f");
	SetSqFunction("log", Log, 2, ". f");
	SetSqFunction("log10", Log10, 2, ". f");

	_RPT0(_CRT_WARN, "------ Math library initialized\n");
}

void Shutdown_Math ()
{
	_RPT0(_CRT_WARN, "------ Math library deinitialized\n");
}

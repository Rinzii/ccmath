/*
 * asmlab golden calibration fixture: Estrin polynomial evaluation.
 * Tool-owned only. Not part of the CCMath public library.
 */

#pragma once

namespace asmlab::golden::poly_estrin
{

inline float load_float(float v)
{
	volatile float storage = v;
	return storage;
}

inline float coeff(unsigned index)
{
	static volatile float table[6] = {
		1.125f, -0.25f, 0.03125f, -0.0078125f, 0.00390625f, -0.001953125f};
	return table[index];
}

inline float poly_estrin(float x)
{
	const float a = coeff(0);
	const float b = coeff(1);
	const float c = coeff(2);
	const float d = coeff(3);
	const float e = coeff(4);
	const float f = coeff(5);
	const float xv = load_float(x);
	const float x2 = load_float(xv * xv);

	const float p0 = load_float(a * xv + b);
	const float p1 = load_float(c * xv + d);
	const float p2 = load_float(e * xv + f);

	return load_float((p0 * x2 + p1) * x2 + p2);
}

} // namespace asmlab::golden::poly_estrin

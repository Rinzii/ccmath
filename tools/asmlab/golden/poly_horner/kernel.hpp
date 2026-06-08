/*
 * asmlab golden calibration fixture: Horner polynomial evaluation.
 * Tool-owned only. Not part of the CCMath public library.
 */

#pragma once

namespace asmlab::golden::poly_horner
{

	inline float load_float(float v)
	{
		volatile float storage = v;
		return storage;
	}

	inline float coeff(unsigned index)
	{
		static volatile float table[6] = { 1.125f, -0.25f, 0.03125f, -0.0078125f, 0.00390625f, -0.001953125f };
		return table[index];
	}

	inline float poly_horner(float x)
	{
		const float xv = load_float(x);
		const float a  = coeff(0);
		const float b  = coeff(1);
		const float c  = coeff(2);
		const float d  = coeff(3);
		const float e  = coeff(4);
		const float f  = coeff(5);
		// Volatile staging preserves serial dependency depth for analysis.
		float t0 = load_float(a * xv + b);
		float t1 = load_float(t0 * xv + c);
		float t2 = load_float(t1 * xv + d);
		float t3 = load_float(t2 * xv + e);
		return load_float(t3 * xv + f);
	}

} // namespace asmlab::golden::poly_horner

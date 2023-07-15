#pragma once

#include "hlsl++_common.h"

namespace hlslpp
{
	template<int X>
	struct hlslpp_nodiscard	swizzle1
	{
		template<int A> friend struct swizzle1;

		// Cast

		hlslpp_inline operator float() const { return f32[X]; }

		#define hlslpp_swizzle1_swizzle(E, A, v) _hlslpp_perm_ps(v, (((IdentityMask >> 2 * E) & 3) << 2 * A) | (IdentityMask & ~((3 << 2 * A))))

		// Assignment

		hlslpp_inline swizzle1& operator = (float f)
		{
			vec = _hlslpp_blend_ps(vec, _hlslpp_set1_ps(f), HLSLPP_COMPONENT_X(X));
			return *this;
		}

		// Revise these functions. Can I not do with swizzle?

		template<int A>
		hlslpp_inline swizzle1& operator = (const swizzle1<A>& s)
		{
			n128 t = _hlslpp_shuffle_ps(s.vec, s.vec, HLSLPP_SHUFFLE_MASK(A, A, A, A));
			vec = _hlslpp_blend_ps(vec, t, HLSLPP_COMPONENT_X(X));
			return *this;
		}

		hlslpp_inline swizzle1& operator = (const swizzle1<X>& s)
		{
			n128 t = _hlslpp_shuffle_ps(s.vec, s.vec, HLSLPP_SHUFFLE_MASK(X, X, X, X));
			vec = _hlslpp_blend_ps(vec, t, HLSLPP_COMPONENT_X(X));
			return *this;
		}

		hlslpp_inline swizzle1& operator = (const float1& f);

		union
		{
			n128 vec;
			float f32[4];
		};
	};

	template<int X, int Y>
	struct hlslpp_nodiscard	swizzle2
	{
		// Helper

		void staticAsserts()
		{
			static_assert(X != Y, "\"l-value specifies const object\" No component can be equal for assignment.");
		}

		#define hlslpp_swizzle2_swizzle(E, F, A, B, v) \
			_hlslpp_perm_ps(v, (((IdentityMask >> 2 * E) & 3) << 2 * A) | \
			                   (((IdentityMask >> 2 * F) & 3) << 2 * B) | \
			                   (IdentityMask & ~((3 << 2 * A) | (3 << 2 * B))))

		#define hlslpp_swizzle2_blend(x, y) _hlslpp_blend_ps(x, y, HLSLPP_COMPONENT_XY(X, Y))

		template<int A, int B>
		hlslpp_inline swizzle2& operator = (const swizzle2<A, B>& s)
		{
			staticAsserts();
			vec = hlslpp_swizzle2_blend(vec, hlslpp_swizzle2_swizzle(A, B, X, Y, s.vec));
			return *this;
		}

		hlslpp_inline swizzle2& operator = (const swizzle2<X, Y>& s)
		{
			staticAsserts();
			vec = hlslpp_swizzle2_blend(vec, hlslpp_swizzle2_swizzle(X, Y, X, Y, s.vec));
			return *this;
		}

		hlslpp_inline swizzle2& operator = (const float2& f);

		n128 vec;
	};

	template<int X, int Y, int Z>
	struct hlslpp_nodiscard	swizzle3
	{
		void staticAsserts()
		{
			static_assert(X != Y && X != Z && Y != Z, "\"l-value specifies const object\" No component can be equal for assignment.");
		}

		#define hlslpp_swizzle3_swizzle(E, F, G, A, B, C, v) \
			_hlslpp_perm_ps(v, (((IdentityMask >> 2 * E) & 3) << 2 * A) | \
			                   (((IdentityMask >> 2 * F) & 3) << 2 * B) | \
			                   (((IdentityMask >> 2 * G) & 3) << 2 * C) | \
			                   (IdentityMask & ~((3 << 2 * A) | (3 << 2 * B) | (3 << 2 * C))))

		#define hlslpp_swizzle3_blend(x, y) _hlslpp_blend_ps(x, y, HLSLPP_COMPONENT_XYZ(X, Y, Z))

		template<int A, int B, int C>
		hlslpp_inline swizzle3& operator = (const swizzle3<A, B, C>& s)
		{
			staticAsserts();
			vec = hlslpp_swizzle3_blend(vec, hlslpp_swizzle3_swizzle(A, B, C, X, Y, Z, s.vec));
			return *this;
		}

		hlslpp_inline swizzle3& operator = (const swizzle3<X, Y, Z>& s)
		{
			staticAsserts();
			vec = hlslpp_swizzle3_blend(vec, hlslpp_swizzle3_swizzle(X, Y, Z, X, Y, Z, s.vec));
			return *this;
		}

		hlslpp_inline swizzle3& operator = (const float3& f);

		n128 vec;
	};

	template<int X, int Y, int Z, int W>
	struct hlslpp_nodiscard swizzle4
	{
		void staticAsserts()
		{
			static_assert(X != Y && X != Z && X != W && Y != Z && Y != W && Z != W, "\"l-value specifies const object\" No component can be equal for assignment.");
		}

		#define hlslpp_swizzle4_swizzle(E, F, G, H, A, B, C, D, v) \
			_hlslpp_perm_ps(v, (((IdentityMask >> 2 * E) & 3) << (2 * A)) | \
			                   (((IdentityMask >> 2 * F) & 3) << (2 * B)) | \
			                   (((IdentityMask >> 2 * G) & 3) << (2 * C)) | \
			                   (((IdentityMask >> 2 * H) & 3) << (2 * D)))

		// Assignment

		template<int A, int B, int C, int D>
		hlslpp_inline swizzle4& operator = (const swizzle4<A, B, C, D>& s)
		{
			staticAsserts();
			vec = hlslpp_swizzle4_swizzle(A, B, C, D, X, Y, Z, W, s.vec);
			return *this;
		}

		hlslpp_inline swizzle4& operator = (const float4& f);

		n128 vec;
	};

	struct hlslpp_nodiscard float1
	{
		hlslpp_inline float1() hlslpp_noexcept : vec(_hlslpp_setzero_ps()) {}
		hlslpp_inline float1(const float1& f) hlslpp_noexcept : vec(f.vec) {}
		explicit hlslpp_inline float1(n128 vec) hlslpp_noexcept : vec(vec) {}

		template<typename T>
		hlslpp_inline float1(T f, hlslpp_enable_if_number(T)) hlslpp_noexcept : vec(_hlslpp_set_ps(float(f), 0.0f, 0.0f, 0.0f)) {}

		template<int X> hlslpp_inline float1(const swizzle1<X>& s) hlslpp_noexcept
			: vec(hlslpp_swizzle1_swizzle(X, 0, s.vec)) {}

		hlslpp_inline float1(const int1& i) hlslpp_noexcept;

		hlslpp_inline float1(const uint1& i) hlslpp_noexcept;

		hlslpp_inline operator float() const { return f32[0]; }

		hlslpp_inline float1& operator = (const float1& f) hlslpp_noexcept { vec = f.vec; return *this; }

		hlslpp_inline float1(float1&& f) hlslpp_noexcept : vec(f.vec) {}
		hlslpp_inline float1& operator = (float1&& f) hlslpp_noexcept { vec = f.vec; return *this; }

		float& operator[](int N)
		{
			hlslpp_assert(N == 0);
			return f32[N];
		}

		const float& operator[](int N) const
		{
			hlslpp_assert(N == 0);
			return f32[N];
		}

		union
		{
			n128 vec;
			float f32[1];
			#include "swizzle/hlsl++_vector_float_x.h"
		};
	};

	struct hlslpp_nodiscard	float2
	{
		hlslpp_inline float2() hlslpp_noexcept : vec(_hlslpp_setzero_ps()) {}
		hlslpp_inline float2(const float2& f) hlslpp_noexcept : vec(f.vec) {}
		explicit hlslpp_inline float2(n128 vec) hlslpp_noexcept : vec(vec) {}
		explicit hlslpp_inline float2(const float1& f) hlslpp_noexcept : vec(_hlslpp_perm_xxxx_ps(f.vec)) {}

		template<typename T>
		hlslpp_inline float2(T f, hlslpp_enable_if_number(T)) hlslpp_noexcept : vec(_hlslpp_set_ps(float(f), float(f), 0.0f, 0.0f)) {}

		template<typename T1, typename T2>
		hlslpp_inline float2(T1 f1, T2 f2, hlslpp_enable_if_number_2(T1, T2)) hlslpp_noexcept : vec(_hlslpp_set_ps(float(f1), float(f2), 0.0f, 0.0f)) {}

		hlslpp_inline float2(const float1& f1, const float1& f2) hlslpp_noexcept { vec = _hlslpp_blend_ps(f1.vec, _hlslpp_perm_xxxx_ps(f2.vec), HLSLPP_BLEND_MASK(1, 0, 1, 1)); }

		template<int X, int Y> hlslpp_inline float2(const swizzle2<X, Y>& s) hlslpp_noexcept
			: vec(hlslpp_swizzle2_swizzle(X, Y, 0, 1, s.vec)) {}

		hlslpp_inline float2(const int2& i) hlslpp_noexcept;

		hlslpp_inline float2(const uint2& i) hlslpp_noexcept;

		hlslpp_inline float2& operator = (const float2& f) hlslpp_noexcept { vec = f.vec; return *this; }

		hlslpp_inline float2(float2&& f) hlslpp_noexcept : vec(f.vec) {}
		hlslpp_inline float2& operator = (float2&& f) hlslpp_noexcept { vec = f.vec; return *this; }

		float& operator[](int N)
		{
			hlslpp_assert(N >= 0 && N <= 1);
			return f32[N];
		}

		const float& operator[](int N) const
		{
			hlslpp_assert(N >= 0 && N <= 1);
			return f32[N];
		}

		union
		{
			n128 vec;
			float f32[2];
			#include "swizzle/hlsl++_vector_float_x.h"
			#include "swizzle/hlsl++_vector_float_y.h"
		};
	};

	struct hlslpp_nodiscard	float3
	{
		hlslpp_inline float3() hlslpp_noexcept : vec(_hlslpp_setzero_ps()) {}
		hlslpp_inline float3(const float3& f) hlslpp_noexcept : vec(f.vec) {}
		explicit hlslpp_inline float3(n128 vec) hlslpp_noexcept : vec(vec) {}
		explicit hlslpp_inline float3(const float1& f) hlslpp_noexcept : vec(_hlslpp_perm_xxxx_ps(f.vec)) {}

		template<typename T>
		hlslpp_inline float3(T f, hlslpp_enable_if_number(T)) hlslpp_noexcept : vec(_hlslpp_set_ps(float(f), float(f), float(f), 0.0f)) {}

		template<typename T1, typename T2, typename T3>
		hlslpp_inline float3(T1 f1, T2 f2, T3 f3, hlslpp_enable_if_number_3(T1, T2, T3)) hlslpp_noexcept : vec(_hlslpp_set_ps(float(f1), float(f2), float(f3), 0.0f)) {}

		hlslpp_inline float3(const float1& f1, const float1& f2, const float1& f3) hlslpp_noexcept
		{ vec = _hlslpp_blend_ps(_hlslpp_shuf_xxxx_ps(f1.vec, f3.vec), _hlslpp_perm_xxxx_ps(f2.vec), HLSLPP_BLEND_MASK(1, 0, 1, 0)); }

		hlslpp_inline float3(const float2& f1, const float1& f2) hlslpp_noexcept { vec = _hlslpp_shuf_xyxx_ps(f1.vec, f2.vec); }
		hlslpp_inline float3(const float1& f1, const float2& f2) hlslpp_noexcept { vec = _hlslpp_blend_ps(f1.vec, _hlslpp_perm_xxyx_ps(f2.vec), HLSLPP_BLEND_MASK(1, 0, 0, 1)); }

		template<int X, int Y, int Z>
		hlslpp_inline float3(const swizzle3<X, Y, Z>& s) hlslpp_noexcept
			: vec(hlslpp_swizzle3_swizzle(X, Y, Z, 0, 1, 2, s.vec)) {}

		hlslpp_inline float3(const int3& i) hlslpp_noexcept;

		hlslpp_inline float3(const uint3& i) hlslpp_noexcept;

		hlslpp_inline float3& operator = (const float3& f) hlslpp_noexcept { vec = f.vec; return *this; }

		hlslpp_inline float3(float3&& f) hlslpp_noexcept : vec(f.vec) {}
		hlslpp_inline float3& operator = (float3&& f) hlslpp_noexcept { vec = f.vec; return *this; }

		float& operator[](int N)
		{
			hlslpp_assert(N >= 0 && N <= 2);
			return f32[N];
		}

		const float& operator[](int N) const
		{
			hlslpp_assert(N >= 0 && N <= 2);
			return f32[N];
		}

		union
		{
			n128 vec;
			float f32[3];
			#include "swizzle/hlsl++_vector_float_x.h"
			#include "swizzle/hlsl++_vector_float_y.h"
			#include "swizzle/hlsl++_vector_float_z.h"
		};
	};

	struct hlslpp_nodiscard	float4
	{
		hlslpp_inline float4() hlslpp_noexcept : vec(_hlslpp_setzero_ps()) {}
		hlslpp_inline float4(const float4& f) hlslpp_noexcept : vec(f.vec) {}
		explicit hlslpp_inline float4(n128 vec) hlslpp_noexcept : vec(vec) {}
		explicit hlslpp_inline float4(const float1& f) hlslpp_noexcept : vec(_hlslpp_perm_xxxx_ps(f.vec)) {}

		template<typename T>
		float4(T f, hlslpp_enable_if_number(T)) hlslpp_noexcept : vec(_hlslpp_set1_ps(float(f))) {}

		template<typename T1, typename T2, typename T3, typename T4>
		hlslpp_inline float4(T1 f1, T2 f2, T3 f3, T4 f4, hlslpp_enable_if_number_4(T1, T2, T3, T4)) hlslpp_noexcept : vec(_hlslpp_set_ps(float(f1), float(f2), float(f3), float(f4))) {}

		hlslpp_inline float4(const float1& f1, const float1& f2, const float1& f3, const float1& f4) hlslpp_noexcept 
		{ vec = _hlslpp_blend_ps(_hlslpp_shuf_xxxx_ps(f1.vec, f3.vec), _hlslpp_shuf_xxxx_ps(f2.vec, f4.vec), HLSLPP_BLEND_MASK(1, 0, 1, 0)); }

		hlslpp_inline float4(const float2& f1, const float1& f2, const float1& f3) hlslpp_noexcept
		{ vec = _hlslpp_blend_ps(_hlslpp_shuf_xyxx_ps(f1.vec, f2.vec), _hlslpp_perm_xxxx_ps(f3.vec), HLSLPP_BLEND_MASK(1, 1, 1, 0)); }

		hlslpp_inline float4(const float1& f1, const float2& f2, const float1& f3) hlslpp_noexcept
		{ vec = _hlslpp_blend_ps(_hlslpp_shuf_xxxx_ps(f1.vec, f3.vec), _hlslpp_perm_xxyx_ps(f2.vec), HLSLPP_BLEND_MASK(1, 0, 0, 1)); }

		hlslpp_inline float4(const float1& f1, const float1& f2, const float2& f3) hlslpp_noexcept
		{ vec = _hlslpp_blend_ps(_hlslpp_shuf_xxxy_ps(f1.vec, f3.vec), _hlslpp_perm_xxxx_ps(f2.vec), HLSLPP_BLEND_MASK(1, 0, 1, 1)); }

		hlslpp_inline float4(const float2& f1, const float2& f2) hlslpp_noexcept { vec = _hlslpp_shuf_xyxy_ps(f1.vec, f2.vec); }

		hlslpp_inline float4(const float1& f1, const float3& f2) hlslpp_noexcept { vec = _hlslpp_blend_ps(f1.vec, _hlslpp_perm_xxyz_ps(f2.vec), HLSLPP_BLEND_MASK(1, 0, 0, 0)); }
		hlslpp_inline float4(const float3& f1, const float1& f2) hlslpp_noexcept { vec = _hlslpp_blend_ps(f1.vec, _hlslpp_perm_xxxx_ps(f2.vec), HLSLPP_BLEND_MASK(1, 1, 1, 0)); }

		template<int X, int Y, int Z, int W>
		hlslpp_inline float4(const swizzle4<X, Y, Z, W>& s) hlslpp_noexcept
			: vec(hlslpp_swizzle4_swizzle(X, Y, Z, W, 0, 1, 2, 3, s.vec)) {}

		hlslpp_inline float4(const int4& i) hlslpp_noexcept;

		hlslpp_inline float4(const uint4& i) hlslpp_noexcept;

		hlslpp_inline float4& operator = (const float4& f) hlslpp_noexcept { vec = f.vec; return *this; }

		hlslpp_inline float4(float4&& f) hlslpp_noexcept : vec(f.vec) {}
		hlslpp_inline float4& operator = (float4&& f) hlslpp_noexcept { vec = f.vec; return *this; }

		float& operator[](int N)
		{
			hlslpp_assert(N >= 0 && N <= 3);
			return f32[N];
		}

		const float& operator[](int N) const
		{
			hlslpp_assert(N >= 0 && N <= 3);
			return f32[N];
		}

		union
		{
			n128 vec;
			float f32[4];
			#include "swizzle/hlsl++_vector_float_x.h"
			#include "swizzle/hlsl++_vector_float_y.h"
			#include "swizzle/hlsl++_vector_float_z.h"
			#include "swizzle/hlsl++_vector_float_w.h"
		};
	};
};
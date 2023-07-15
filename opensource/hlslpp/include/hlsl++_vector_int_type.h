#pragma once

#include "hlsl++_common.h"

namespace hlslpp
{
	template<int X>
	struct hlslpp_nodiscard iswizzle1
	{
		template<int A> friend struct iswizzle1;
	
		// Cast
	
		hlslpp_inline operator int32_t() const { return i32[X]; }
	
		#define hlslpp_iswizzle1_swizzle(E, A, v) _hlslpp_perm_epi32(v, (((IdentityMask >> 2 * E) & 3) << 2 * A) | (IdentityMask & ~((3 << 2 * A))))

		// Assignment

		hlslpp_inline iswizzle1& operator = (int32_t i)
		{
			vec = _hlslpp_blend_epi32(vec, _hlslpp_set1_epi32(i), HLSLPP_COMPONENT_X(X));
			return *this;
		}

		// Revise these functions. Can I not do with swizzle?

		template<int A>
		hlslpp_inline iswizzle1& operator = (const iswizzle1<A>& s)
		{
			n128i t = _hlslpp_shuffle_epi32(s.vec, s.vec, HLSLPP_SHUFFLE_MASK(A, A, A, A));
			vec = _hlslpp_blend_epi32(vec, t, HLSLPP_COMPONENT_X(X));
			return *this;
		}

		hlslpp_inline iswizzle1& operator = (const iswizzle1<X>& s)
		{
			n128i t = _hlslpp_shuffle_epi32(s.vec, s.vec, HLSLPP_SHUFFLE_MASK(X, X, X, X));
			vec = _hlslpp_blend_epi32(vec, t, HLSLPP_COMPONENT_X(X));
			return *this;
		}

		hlslpp_inline iswizzle1& operator = (const int1& i);

		union
		{
			n128i vec;
			int32_t i32[4];
		};
	};


	template<int X, int Y>
	struct hlslpp_nodiscard iswizzle2
	{
		// Helper

		void staticAsserts()
		{
			static_assert(X != Y, "\"l-value specifies const object\" No component can be equal for assignment.");
		}

		#define hlslpp_iswizzle2_swizzle(E, F, A, B, v) \
			_hlslpp_perm_epi32(v, (((IdentityMask >> 2 * E) & 3) << 2 * A) | \
			                      (((IdentityMask >> 2 * F) & 3) << 2 * B) | \
			                      (IdentityMask & ~((3 << 2 * A) | (3 << 2 * B))))

		// Select based on property mask
		#define hlslpp_iswizzle2_blend(x, y) _hlslpp_blend_epi32(x, y, HLSLPP_COMPONENT_XY(X, Y))

		// Assignment

		template<int A, int B>
		hlslpp_inline iswizzle2& operator = (const iswizzle2<A, B>& s)
		{
			staticAsserts();
			vec = hlslpp_iswizzle2_blend(vec, hlslpp_iswizzle2_swizzle(A, B, X, Y, s.vec));
			return *this;
		}

		hlslpp_inline iswizzle2& operator = (const iswizzle2<X, Y>& s)
		{
			staticAsserts();
			vec = hlslpp_iswizzle2_blend(vec, hlslpp_iswizzle2_swizzle(X, Y, X, Y, s.vec));
			return *this;
		}

		hlslpp_inline iswizzle2& operator = (const int2& i);

		n128i vec;
	};

	template<int X, int Y, int Z>
	struct hlslpp_nodiscard iswizzle3
	{
		// Helper

		void staticAsserts()
		{
			static_assert(X != Y && X != Z && Y != Z, "\"l-value specifies const object\" No component can be equal for assignment.");
		}

		#define hlslpp_iswizzle3_swizzle(E, F, G, A, B, C, v) \
			_hlslpp_perm_epi32(v, (((IdentityMask >> 2 * E) & 3) << 2 * A) | \
			                      (((IdentityMask >> 2 * F) & 3) << 2 * B) | \
			                      (((IdentityMask >> 2 * G) & 3) << 2 * C) | \
			                      (IdentityMask & ~((3 << 2 * A) | (3 << 2 * B) | (3 << 2 * C))))

		// Select based on property mask
		#define hlslpp_iswizzle3_blend(x, y) _hlslpp_blend_epi32(x, y, HLSLPP_COMPONENT_XYZ(X, Y, Z))

		// Assignment

		template<int A, int B, int C>
		hlslpp_inline iswizzle3& operator = (const iswizzle3<A, B, C>& s)
		{
			staticAsserts();
			vec = hlslpp_iswizzle3_blend(vec, hlslpp_iswizzle3_swizzle(A, B, C, X, Y, Z, s.vec));
			return *this;
		}

		hlslpp_inline iswizzle3& operator = (const iswizzle3<X, Y, Z>& s)
		{
			staticAsserts();
			vec = hlslpp_iswizzle3_blend(vec, hlslpp_iswizzle3_swizzle(X, Y, Z, X, Y, Z, s.vec));
			return *this;
		}

		hlslpp_inline iswizzle3& operator = (const int3& i);

		n128i vec;
	};

	template<int X, int Y, int Z, int W>
	struct hlslpp_nodiscard iswizzle4
	{
		// Helper

		void staticAsserts()
		{
			static_assert(X != Y && X != Z && X != W && Y != Z && Y != W && Z != W, "\"l-value specifies const object\" No component can be equal for assignment.");
		}

		#define hlslpp_iswizzle4_swizzle(E, F, G, H, A, B, C, D, v) \
			_hlslpp_perm_epi32(v, (((IdentityMask >> 2 * E) & 3) << (2 * A)) | \
			                      (((IdentityMask >> 2 * F) & 3) << (2 * B)) | \
			                      (((IdentityMask >> 2 * G) & 3) << (2 * C)) | \
			                      (((IdentityMask >> 2 * H) & 3) << (2 * D)))

		// Assignment

		template<int A, int B, int C, int D>
		hlslpp_inline iswizzle4& operator = (const iswizzle4<A, B, C, D>& s)
		{
			staticAsserts();
			vec = hlslpp_iswizzle4_swizzle(A, B, C, D, X, Y, Z, W, s.vec);
			return *this;
		}

		hlslpp_inline iswizzle4& operator = (const int4& i);

		n128i vec;
	};

	struct hlslpp_nodiscard int1
	{
		hlslpp_inline int1() hlslpp_noexcept : vec(_hlslpp_setzero_epi32()) {}
		hlslpp_inline int1(const int1& i) hlslpp_noexcept : vec(i.vec) {}
		explicit hlslpp_inline int1(n128i vec) hlslpp_noexcept : vec(vec) {}

		template<typename T>
		hlslpp_inline int1(T i, hlslpp_enable_if_number(T)) hlslpp_noexcept : vec(_hlslpp_set_epi32((int)i, 0, 0, 0)) {}

		template<int X> hlslpp_inline int1(const iswizzle1<X>& s) hlslpp_noexcept
			: vec(hlslpp_iswizzle1_swizzle(X, 0, s.vec)) {}

		hlslpp_inline int1(const float1& f) hlslpp_noexcept;

		hlslpp_inline int1& operator = (const int1& i) hlslpp_noexcept { vec = i.vec; return *this; }

		hlslpp_inline int1(int1&& i) hlslpp_noexcept : vec(i.vec) {}
		hlslpp_inline int1& operator = (int1&& i) hlslpp_noexcept { vec = i.vec; return *this; }

		hlslpp_inline operator int32_t() const { return i32[0]; }

		int32_t& operator[](int N)
		{
			hlslpp_assert(N == 0);
			return i32[N];
		}

		const int32_t& operator[](int N) const
		{
			hlslpp_assert(N == 0);
			return i32[N];
		}

		union
		{
			n128i vec;
			int32_t i32[1];
			#include "swizzle/hlsl++_vector_int_x.h"
		};
	};

	
	struct hlslpp_nodiscard int2
	{
		// Constructors

		hlslpp_inline int2() hlslpp_noexcept : vec(_hlslpp_setzero_epi32()) {}
		hlslpp_inline int2(const int2& i) hlslpp_noexcept : vec(i.vec) {}
		explicit hlslpp_inline int2(n128i vec) hlslpp_noexcept : vec(vec) {}
		explicit hlslpp_inline int2(const int1& i) hlslpp_noexcept : vec(_hlslpp_perm_xxxx_epi32(i.vec)) {}

		hlslpp_inline int2(int32_t i) hlslpp_noexcept : vec(_hlslpp_set_epi32(i, i, 0, 0)) {}

		template<typename T1, typename T2>
		hlslpp_inline int2(T1 i1, T2 i2, hlslpp_enable_if_number_2(T1, T2)) hlslpp_noexcept : vec(_hlslpp_set_epi32((int)i1, (int)i2, 0, 0)) {}

		hlslpp_inline int2(const int1& i1, const int1& i2) hlslpp_noexcept { vec = _hlslpp_blend_epi32(i1.vec, _hlslpp_perm_xxxx_epi32(i2.vec), HLSLPP_BLEND_MASK(1, 0, 1, 1)); }
		
		template<int X, int Y> hlslpp_inline int2(const iswizzle2<X, Y>& s) hlslpp_noexcept
			: vec(hlslpp_iswizzle2_swizzle(X, Y, 0, 1, s.vec)) {}

		hlslpp_inline int2(const float2& f) hlslpp_noexcept;

		hlslpp_inline int2& operator = (const int2& i) hlslpp_noexcept { vec = i.vec; return *this; }

		hlslpp_inline int2(int2&& i) hlslpp_noexcept : vec(i.vec) {}
		hlslpp_inline int2& operator = (int2&& i) hlslpp_noexcept { vec = i.vec; return *this; }

		int32_t& operator[](int N)
		{
			hlslpp_assert(N >= 0 && N <= 1);
			return i32[N];
		}

		const int32_t& operator[](int N) const
		{
			hlslpp_assert(N >= 0 && N <= 1);
			return i32[N];
		}

		union
		{
			n128i vec;
			int32_t i32[2];
			#include "swizzle/hlsl++_vector_int_x.h"
			#include "swizzle/hlsl++_vector_int_y.h"
		};
	};

	struct hlslpp_nodiscard int3
	{
		// Constructors

		hlslpp_inline int3() hlslpp_noexcept : vec(_hlslpp_setzero_epi32()) {}
		hlslpp_inline int3(const int3& i) hlslpp_noexcept : vec(i.vec) {}
		explicit hlslpp_inline int3(n128i vec) hlslpp_noexcept : vec(vec) {}

		explicit hlslpp_inline int3(const int1& i) hlslpp_noexcept : vec(_hlslpp_perm_xxxx_epi32(i.vec)) {}

		hlslpp_inline int3(int32_t i) hlslpp_noexcept : vec(_hlslpp_set_epi32(i, i, i, 0)) {}

		template<typename T1, typename T2, typename T3>
		hlslpp_inline int3(T1 i1, T2 i2, T3 i3, hlslpp_enable_if_number_3(T1, T2, T3)) hlslpp_noexcept : vec(_hlslpp_set_epi32((int)i1, (int)i2, (int)i3, 0)) {}

		hlslpp_inline int3(const int1& i1, const int1& i2, const int1& i3) hlslpp_noexcept { vec = _hlslpp_blend_epi32(_hlslpp_shuf_xxxx_epi32(i1.vec, i3.vec), _hlslpp_perm_xxxx_epi32(i2.vec), HLSLPP_BLEND_MASK(1, 0, 1, 0)); }

		hlslpp_inline int3(const int2& i1, const int1& i2) hlslpp_noexcept { vec = _hlslpp_shuf_xyxx_epi32(i1.vec, i2.vec); }
		hlslpp_inline int3(const int1& i1, const int2& i2) hlslpp_noexcept { vec = _hlslpp_blend_epi32(i1.vec, _hlslpp_perm_xxyx_epi32(i2.vec), HLSLPP_BLEND_MASK(1, 0, 0, 1)); }

		template<int X, int Y, int Z>
		hlslpp_inline int3(const iswizzle3<X, Y, Z>& s) hlslpp_noexcept
			: vec(hlslpp_iswizzle3_swizzle(X, Y, Z, 0, 1, 2, s.vec)) {}

		hlslpp_inline int3(const float3& f) hlslpp_noexcept;

		hlslpp_inline int3& operator = (const int3& i) hlslpp_noexcept { vec = i.vec; return *this; }

		hlslpp_inline int3(int3&& i) hlslpp_noexcept : vec(i.vec) {}
		hlslpp_inline int3& operator = (int3&& i) hlslpp_noexcept { vec = i.vec; return *this; }

		int32_t& operator[](int N)
		{
			hlslpp_assert(N >= 0 && N <= 2);
			return i32[N];
		}

		const int32_t& operator[](int N) const
		{
			hlslpp_assert(N >= 0 && N <= 2);
			return i32[N];
		}

		union
		{
			n128i vec;
			int32_t i32[3];
			#include "swizzle/hlsl++_vector_int_x.h"
			#include "swizzle/hlsl++_vector_int_y.h"
			#include "swizzle/hlsl++_vector_int_z.h"
		};
	};

	struct hlslpp_nodiscard int4
	{
		hlslpp_inline int4() hlslpp_noexcept : vec(_hlslpp_setzero_epi32()) {}
		hlslpp_inline int4(const int4& i) hlslpp_noexcept : vec(i.vec) {}
		explicit hlslpp_inline int4(n128i vec) hlslpp_noexcept : vec(vec) {}

		explicit hlslpp_inline int4(const int1& i) hlslpp_noexcept : vec(_hlslpp_perm_xxxx_epi32(i.vec)) {}

		hlslpp_inline int4(int32_t i) hlslpp_noexcept : vec(_hlslpp_set1_epi32(i)) {}

		template<typename T1, typename T2, typename T3, typename T4>
		hlslpp_inline int4(T1 i1, T2 i2, T3 i3, T4 i4, hlslpp_enable_if_number_4(T1, T2, T3, T4)) hlslpp_noexcept : vec(_hlslpp_set_epi32((int)i1, (int)i2, (int)i3, (int)i4)) {}

		hlslpp_inline int4(const int1& i1, const int1& i2, const int1& i3, const int1& i4) hlslpp_noexcept
		{ vec = _hlslpp_blend_epi32(_hlslpp_shuf_xxxx_epi32(i1.vec, i3.vec), _hlslpp_shuf_xxxx_epi32(i2.vec, i4.vec), HLSLPP_BLEND_MASK(1, 0, 1, 0)); }
		
		hlslpp_inline int4(const int2& i1, const int1& i2, const int1& i3) hlslpp_noexcept
		{ vec = _hlslpp_blend_epi32(_hlslpp_shuf_xyxx_epi32(i1.vec, i2.vec), _hlslpp_perm_xxxx_epi32(i3.vec), HLSLPP_BLEND_MASK(1, 1, 1, 0)); }

		hlslpp_inline int4(const int1& i1, const int2& i2, const int1& i3) hlslpp_noexcept
		{ vec = _hlslpp_blend_epi32(_hlslpp_shuf_xxxx_epi32(i1.vec, i3.vec), _hlslpp_perm_xxyx_epi32(i2.vec), HLSLPP_BLEND_MASK(1, 0, 0, 1)); }

		hlslpp_inline int4(const int1& i1, const int1& i2, const int2& i3) hlslpp_noexcept
		{ vec = _hlslpp_blend_epi32(_hlslpp_shuf_xxxy_epi32(i1.vec, i3.vec), _hlslpp_perm_xxxx_epi32(i2.vec), HLSLPP_BLEND_MASK(1, 0, 1, 1)); }

		hlslpp_inline int4(const int2& i1, const int2& f2) hlslpp_noexcept { vec = _hlslpp_shuf_xyxy_epi32(i1.vec, f2.vec); }

		hlslpp_inline int4(const int1& i1, const int3& i2) hlslpp_noexcept { vec = _hlslpp_blend_epi32(i1.vec, _hlslpp_perm_xxyz_epi32(i2.vec), HLSLPP_BLEND_MASK(1, 0, 0, 0)); }
		hlslpp_inline int4(const int3& i1, const int1& i2) hlslpp_noexcept { vec = _hlslpp_blend_epi32(i1.vec, _hlslpp_perm_xxxx_epi32(i2.vec), HLSLPP_BLEND_MASK(1, 1, 1, 0)); }

		template<int X, int Y, int Z, int W>
		hlslpp_inline int4(const iswizzle4<X, Y, Z, W>& s) hlslpp_noexcept
			: vec(hlslpp_iswizzle4_swizzle(X, Y, Z, W, 0, 1, 2, 3, s.vec)) {}

		hlslpp_inline int4(const float4& f) hlslpp_noexcept;

		hlslpp_inline int4& operator = (const int4& i) hlslpp_noexcept { vec = i.vec; return *this; }

		hlslpp_inline int4(int4&& i) hlslpp_noexcept : vec(i.vec) {}
		hlslpp_inline int4& operator = (int4&& i) hlslpp_noexcept { vec = i.vec; return *this; }

		int32_t& operator[](int N)
		{
			hlslpp_assert(N >= 0 && N <= 3);
			return i32[N];
		}

		const int32_t& operator[](int N) const
		{
			hlslpp_assert(N >= 0 && N <= 3);
			return i32[N];
		}

		union
		{
			n128i vec;
			int32_t i32[4];
			#include "swizzle/hlsl++_vector_int_x.h"
			#include "swizzle/hlsl++_vector_int_y.h"
			#include "swizzle/hlsl++_vector_int_z.h"
			#include "swizzle/hlsl++_vector_int_w.h"
		};
	};
};
#pragma once

#include "hlsl++_common.h"

namespace hlslpp
{
	template<int X>
	struct hlslpp_nodiscard uswizzle1
	{
		template<int A> friend struct uswizzle1;
	
		// Cast
	
		hlslpp_inline operator uint32_t() const { return u32[X]; }
	
		#define hlslpp_uswizzle1_swizzle(E, A, v) _hlslpp_perm_epi32(v, (((IdentityMask >> 2 * E) & 3) << 2 * A) | (IdentityMask & ~((3 << 2 * A))))

		// Assignment

		hlslpp_inline uswizzle1& operator = (uint32_t i)
		{
			vec = _hlslpp_blend_epi32(vec, _hlslpp_set1_epu32(i), HLSLPP_COMPONENT_X(X));
			return *this;
		}

		// Revise these functions. Can I not do with swizzle?

		template<int A>
		hlslpp_inline uswizzle1& operator = (const uswizzle1<A>& s)
		{
			n128u t = _hlslpp_shuffle_epi32(s.vec, s.vec, HLSLPP_SHUFFLE_MASK(A, A, A, A));
			vec = _hlslpp_blend_epi32(vec, t, HLSLPP_COMPONENT_X(X));
			return *this;
		}

		hlslpp_inline uswizzle1& operator = (const uswizzle1<X>& s)
		{
			n128u t = _hlslpp_shuffle_epi32(s.vec, s.vec, HLSLPP_SHUFFLE_MASK(X, X, X, X));
			vec = _hlslpp_blend_epi32(vec, t, HLSLPP_COMPONENT_X(X));
			return *this;
		}

		hlslpp_inline uswizzle1& operator = (const uint1& i);

		union
		{
			n128u vec;
			uint32_t u32[4];
		};
	};
	
	template<int X, int Y>
	struct hlslpp_nodiscard uswizzle2
	{
		// Helper

		void staticAsserts()
		{
			static_assert(X != Y, "\"l-value specifies const object\" No component can be equal for assignment.");
		}

		#define hlslpp_uswizzle2_swizzle(E, F, A, B, v) \
			_hlslpp_perm_epi32(v, (((IdentityMask >> 2 * E) & 3) << 2 * A) | \
			                      (((IdentityMask >> 2 * F) & 3) << 2 * B) | \
			                      (IdentityMask & ~((3 << 2 * A) | (3 << 2 * B))))

		// Select based on property mask
		#define hlslpp_uswizzle2_blend(x, y) _hlslpp_blend_epi32(x, y, HLSLPP_COMPONENT_XY(X, Y))

		// Assignment

		template<int A, int B>
		hlslpp_inline uswizzle2& operator = (const uswizzle2<A, B>& s)
		{
			staticAsserts();
			vec = hlslpp_uswizzle2_blend(vec, hlslpp_uswizzle2_swizzle(A, B, X, Y, s.vec));
			return *this;
		}

		hlslpp_inline uswizzle2& operator = (const uswizzle2<X, Y>& s)
		{
			staticAsserts();
			vec = hlslpp_uswizzle2_blend(vec, hlslpp_uswizzle2_swizzle(X, Y, X, Y, s.vec));
			return *this;
		}

		hlslpp_inline uswizzle2& operator = (const uint2& i);

		n128u vec;
	};
	
	template<int X, int Y, int Z>
	struct hlslpp_nodiscard uswizzle3
	{
		// Helper

		void staticAsserts()
		{
			static_assert(X != Y && X != Z && Y != Z, "\"l-value specifies const object\" No component can be equal for assignment.");
		}

		#define hlslpp_uswizzle3_swizzle(E, F, G, A, B, C, v) \
			_hlslpp_perm_epi32(v, (((IdentityMask >> 2 * E) & 3) << 2 * A) | \
			                      (((IdentityMask >> 2 * F) & 3) << 2 * B) | \
			                      (((IdentityMask >> 2 * G) & 3) << 2 * C) | \
			                      (IdentityMask & ~((3 << 2 * A) | (3 << 2 * B) | (3 << 2 * C))))

		// Select based on property mask
		#define hlslpp_uswizzle3_blend(x, y) _hlslpp_blend_epi32(x, y, HLSLPP_COMPONENT_XYZ(X, Y, Z))

		// Assignment

		template<int A, int B, int C>
		hlslpp_inline uswizzle3& operator = (const uswizzle3<A, B, C>& s)
		{
			staticAsserts();
			vec = hlslpp_uswizzle3_blend(vec, hlslpp_uswizzle3_swizzle(A, B, C, X, Y, Z, s.vec));
			return *this;
		}

		hlslpp_inline uswizzle3& operator = (const uswizzle3<X, Y, Z>& s)
		{
			staticAsserts();
			vec = hlslpp_uswizzle3_blend(vec, hlslpp_uswizzle3_swizzle(X, Y, Z, X, Y, Z, s.vec));
			return *this;
		}

		hlslpp_inline uswizzle3& operator = (const uint3& i);

		n128u vec;
	};

	template<int X, int Y, int Z, int W>
	struct hlslpp_nodiscard uswizzle4
	{
		// Helper

		void staticAsserts()
		{
			static_assert(X != Y && X != Z && X != W && Y != Z && Y != W && Z != W, "\"l-value specifies const object\" No component can be equal for assignment.");
		}

		#define hlslpp_uswizzle4_swizzle(E, F, G, H, A, B, C, D, v) \
			_hlslpp_perm_epi32(v, (((IdentityMask >> 2 * E) & 3) << (2 * A)) | \
			                      (((IdentityMask >> 2 * F) & 3) << (2 * B)) | \
			                      (((IdentityMask >> 2 * G) & 3) << (2 * C)) | \
			                      (((IdentityMask >> 2 * H) & 3) << (2 * D)))

		// Assignment

		template<int A, int B, int C, int D>
		hlslpp_inline uswizzle4& operator = (const uswizzle4<A, B, C, D>& s)
		{
			staticAsserts();
			vec = hlslpp_uswizzle4_swizzle(A, B, C, D, X, Y, Z, W, s.vec);
			return *this;
		}

		hlslpp_inline uswizzle4& operator = (const uint4& i);

		n128u vec;
	};
	
	struct hlslpp_nodiscard uint1
	{
		hlslpp_inline uint1() hlslpp_noexcept : vec(_hlslpp_setzero_epu32()) {}
		hlslpp_inline uint1(const uint1& i) hlslpp_noexcept : vec(i.vec) {}
		explicit hlslpp_inline uint1(n128u vec) hlslpp_noexcept : vec(vec) {}

		template<typename T>
		hlslpp_inline uint1(T i, hlslpp_enable_if_number(T)) hlslpp_noexcept : vec(_hlslpp_set_epu32((unsigned int)i, 0, 0, 0)) {}

		template<int X> hlslpp_inline uint1(const uswizzle1<X>& s) hlslpp_noexcept
			: vec(hlslpp_uswizzle1_swizzle(X, 0, s.vec)) {}

		hlslpp_inline uint1(const float1& f) hlslpp_noexcept;

		hlslpp_inline uint1& operator = (const uint1& i) hlslpp_noexcept { vec = i.vec; return *this; }

		hlslpp_inline uint1(uint1&& i) hlslpp_noexcept : vec(i.vec) {}
		hlslpp_inline uint1& operator = (uint1&& i) hlslpp_noexcept { vec = i.vec; return *this; }

		hlslpp_inline operator uint32_t() const { return u32[0]; }

		uint32_t& operator[](int N)
		{
			hlslpp_assert(N == 0);
			return u32[N];
		}

		const uint32_t& operator[](int N) const
		{
			hlslpp_assert(N == 0);
			return u32[N];
		}

		union
		{
			n128u vec;
			uint32_t u32[1];
			#include "swizzle/hlsl++_vector_uint_x.h"
		};
	};

	struct hlslpp_nodiscard uint2
	{
		// Constructors

		hlslpp_inline uint2() hlslpp_noexcept : vec(_hlslpp_setzero_epu32()) {}
		hlslpp_inline uint2(const uint2& i) hlslpp_noexcept : vec(i.vec) {}
		explicit hlslpp_inline uint2(n128u vec) hlslpp_noexcept : vec(vec) {}
		explicit hlslpp_inline uint2(const uint1& i) hlslpp_noexcept : vec(_hlslpp_perm_xxxx_epi32(i.vec)) {}

		hlslpp_inline uint2(uint32_t i) hlslpp_noexcept : vec(_hlslpp_set_epu32(i, i, 0, 0)) {}

		template<typename T1, typename T2>
		hlslpp_inline uint2(T1 i1, T2 i2, hlslpp_enable_if_number_2(T1, T2)) hlslpp_noexcept : vec(_hlslpp_set_epu32((unsigned int)i1, (unsigned int)i2, 0, 0)) {}

		hlslpp_inline uint2(const uint1& i1, const uint1& i2) hlslpp_noexcept { vec = _hlslpp_blend_epi32(i1.vec, _hlslpp_perm_xxxx_epi32(i2.vec), HLSLPP_BLEND_MASK(1, 0, 1, 1)); }
		
		template<int X, int Y> hlslpp_inline uint2(const uswizzle2<X, Y>& s) hlslpp_noexcept
			: vec(hlslpp_uswizzle2_swizzle(X, Y, 0, 1, s.vec)) {}

		hlslpp_inline uint2(const float2& f) hlslpp_noexcept;

		hlslpp_inline uint2& operator = (const uint2& i) hlslpp_noexcept { vec = i.vec; return *this; }

		hlslpp_inline uint2(uint2&& i) hlslpp_noexcept : vec(i.vec) {}
		hlslpp_inline uint2& operator = (uint2&& i) hlslpp_noexcept { vec = i.vec; return *this; }

		uint32_t& operator[](int N)
		{
			hlslpp_assert(N >= 0 && N <= 1);
			return u32[N];
		}

		const uint32_t& operator[](int N) const
		{
			hlslpp_assert(N >= 0 && N <= 1);
			return u32[N];
		}

		union
		{
			n128u vec;
			uint32_t u32[2];
			#include "swizzle/hlsl++_vector_uint_x.h"
			#include "swizzle/hlsl++_vector_uint_y.h"
		};
	};
	
	struct hlslpp_nodiscard uint3
	{
		// Constructors

		hlslpp_inline uint3() hlslpp_noexcept : vec(_hlslpp_setzero_epu32()) {}
		hlslpp_inline uint3(const uint3& i) hlslpp_noexcept : vec(i.vec) {}
		explicit hlslpp_inline uint3(n128u vec) hlslpp_noexcept : vec(vec) {}

		explicit hlslpp_inline uint3(const uint1& i) hlslpp_noexcept : vec(_hlslpp_perm_xxxx_epi32(i.vec)) {}

		hlslpp_inline uint3(uint32_t i) hlslpp_noexcept : vec(_hlslpp_set_epu32(i, i, i, 0)) {}

		template<typename T1, typename T2, typename T3>
		hlslpp_inline uint3(T1 i1, T2 i2, T3 i3, hlslpp_enable_if_number_3(T1, T2, T3)) hlslpp_noexcept : vec(_hlslpp_set_epu32((unsigned int)i1, (unsigned int)i2, (unsigned int)i3, 0)) {}

		hlslpp_inline uint3(const uint1& i1, const uint1& i2, const uint1& i3) hlslpp_noexcept 
		{ vec = _hlslpp_blend_epi32(_hlslpp_shuf_xxxx_epi32(i1.vec, i3.vec), _hlslpp_perm_xxxx_epi32(i2.vec), HLSLPP_BLEND_MASK(1, 0, 1, 0)); }

		hlslpp_inline uint3(const uint2& i1, const uint1& i2) hlslpp_noexcept { vec = _hlslpp_shuf_xyxx_epi32(i1.vec, i2.vec); }
		hlslpp_inline uint3(const uint1& i1, const uint2& i2) hlslpp_noexcept { vec = _hlslpp_blend_epi32(i1.vec, _hlslpp_perm_xxyx_epi32(i2.vec), HLSLPP_BLEND_MASK(1, 0, 0, 1)); }

		template<int X, int Y, int Z>
		hlslpp_inline uint3(const uswizzle3<X, Y, Z>& s) hlslpp_noexcept
			: vec(hlslpp_uswizzle3_swizzle(X, Y, Z, 0, 1, 2, s.vec)) {}

		hlslpp_inline uint3(const float3& f) hlslpp_noexcept;

		hlslpp_inline uint3& operator = (const uint3& i) hlslpp_noexcept { vec = i.vec; return *this; }

		hlslpp_inline uint3(uint3&& i) hlslpp_noexcept : vec(i.vec) {}
		hlslpp_inline uint3& operator = (uint3&& i) hlslpp_noexcept { vec = i.vec; return *this; }

		uint32_t& operator[](int N)
		{
			hlslpp_assert(N >= 0 && N <= 2);
			return u32[N];
		}

		const uint32_t& operator[](int N) const
		{
			hlslpp_assert(N >= 0 && N <= 2);
			return u32[N];
		}

		union
		{
			n128u vec;
			uint32_t u32[3];
			#include "swizzle/hlsl++_vector_uint_x.h"
			#include "swizzle/hlsl++_vector_uint_y.h"
			#include "swizzle/hlsl++_vector_uint_z.h"
		};
	};

	struct hlslpp_nodiscard uint4
	{
		hlslpp_inline uint4() hlslpp_noexcept : vec(_hlslpp_setzero_epu32()) {}
		hlslpp_inline uint4(const uint4& i) hlslpp_noexcept : vec(i.vec) {}
		explicit hlslpp_inline uint4(n128u vec) hlslpp_noexcept : vec(vec) {}

		explicit hlslpp_inline uint4(const uint1& i) hlslpp_noexcept : vec(_hlslpp_perm_xxxx_epi32(i.vec)) {}

		hlslpp_inline uint4(uint32_t i) hlslpp_noexcept : vec(_hlslpp_set1_epu32(i)) {}

		template<typename T1, typename T2, typename T3, typename T4>
		hlslpp_inline uint4(T1 i1, T2 i2, T3 i3, T4 i4, hlslpp_enable_if_number_4(T1, T2, T3, T4)) hlslpp_noexcept : vec(_hlslpp_set_epu32((unsigned int)i1, (unsigned int)i2, (unsigned int)i3, (unsigned int)i4)) {}

		hlslpp_inline uint4(const uint1& i1, const uint1& i2, const uint1& i3, const uint1& i4) hlslpp_noexcept 
		{ vec = _hlslpp_blend_epi32(_hlslpp_shuf_xxxx_epi32(i1.vec, i3.vec), _hlslpp_shuf_xxxx_epi32(i2.vec, i4.vec), HLSLPP_BLEND_MASK(1, 0, 1, 0)); }
		
		hlslpp_inline uint4(const uint2& i1, const uint1& i2, const uint1& i3) hlslpp_noexcept 
		{ vec = _hlslpp_blend_epi32(_hlslpp_shuf_xyxx_epi32(i1.vec, i2.vec), _hlslpp_perm_xxxx_epi32(i3.vec), HLSLPP_BLEND_MASK(1, 1, 1, 0)); }

		hlslpp_inline uint4(const uint1& i1, const uint2& i2, const uint1& i3) hlslpp_noexcept 
		{ vec = _hlslpp_blend_epi32(_hlslpp_shuf_xxxx_epi32(i1.vec, i3.vec), _hlslpp_perm_xxyx_epi32(i2.vec), HLSLPP_BLEND_MASK(1, 0, 0, 1)); }

		hlslpp_inline uint4(const uint1& i1, const uint1& i2, const uint2& i3) hlslpp_noexcept 
		{ vec = _hlslpp_blend_epi32(_hlslpp_shuf_xxxy_epi32(i1.vec, i3.vec), _hlslpp_perm_xxxx_epi32(i2.vec), HLSLPP_BLEND_MASK(1, 0, 1, 1)); }

		hlslpp_inline uint4(const uint2& i1, const uint2& f2) hlslpp_noexcept { vec = _hlslpp_shuf_xyxy_epi32(i1.vec, f2.vec); }

		hlslpp_inline uint4(const uint1& i1, const uint3& i2) hlslpp_noexcept { vec = _hlslpp_blend_epi32(i1.vec, _hlslpp_perm_xxyz_epi32(i2.vec), HLSLPP_BLEND_MASK(1, 0, 0, 0)); }
		hlslpp_inline uint4(const uint3& i1, const uint1& i2) hlslpp_noexcept { vec = _hlslpp_blend_epi32(i1.vec, _hlslpp_perm_xxxx_epi32(i2.vec), HLSLPP_BLEND_MASK(1, 1, 1, 0)); }

		template<int X, int Y, int Z, int W>
		hlslpp_inline uint4(const uswizzle4<X, Y, Z, W>& s) hlslpp_noexcept
			: vec(hlslpp_uswizzle4_swizzle(X, Y, Z, W, 0, 1, 2, 3, s.vec)) {}

		hlslpp_inline uint4(const float4& f) hlslpp_noexcept;

		hlslpp_inline uint4& operator = (const uint4& i) hlslpp_noexcept { vec = i.vec; return *this; }

		hlslpp_inline uint4(uint4&& i) hlslpp_noexcept : vec(i.vec) {}
		hlslpp_inline uint4& operator = (uint4&& i) hlslpp_noexcept { vec = i.vec; return *this; }

		uint32_t& operator[](int N)
		{
			hlslpp_assert(N >= 0 && N <= 3);
			return u32[N];
		}

		const uint32_t& operator[](int N) const
		{
			hlslpp_assert(N >= 0 && N <= 3);
			return u32[N];
		}

		union
		{
			n128u vec;
			uint32_t u32[4];
			#include "swizzle/hlsl++_vector_uint_x.h"
			#include "swizzle/hlsl++_vector_uint_y.h"
			#include "swizzle/hlsl++_vector_uint_z.h"
			#include "swizzle/hlsl++_vector_uint_w.h"
		};
	};
};
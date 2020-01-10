#pragma once
#include "Types.h"
namespace PointerMath
{

	inline void* AlignForward(void* a_pAddr, uint8 a_alignment)
	{
		return reinterpret_cast<void*>
			(
				(uintptr)a_pAddr
				+
				(uintptr)a_alignment - 1 // Add 
				&
				(uintptr)~(a_alignment - 1) // Round Down to next multiple
			);
	}

	inline uint8 AlignForwardAdjustment(const void* a_pAddress, uint8 a_alignment)
	{
		const uint8 adjustment = a_alignment - (reinterpret_cast<uintptr>(a_pAddress)& static_cast<uintptr>(a_alignment - 1));

		if (adjustment == a_alignment) // already aligned
			return 0;

		return adjustment;
	}

	inline uint8 AlignForwardWithHeader(const void* a_pAddress, uint8 a_alignment, uint8 a_headerSize)
	{
		uint8 adjustment = AlignForwardAdjustment(a_pAddress, a_alignment);

		if(adjustment < a_headerSize)
		{
			const uint8 extra = a_headerSize - adjustment;

			adjustment += a_alignment * (extra / a_alignment);

			if(extra % adjustment > 0)
			{
				adjustment += a_alignment;
			}
		}

		return adjustment;
	}
	
}
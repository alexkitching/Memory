#pragma once
#include "Types.h"

//------------
// Description
//--------------
// Pointer Math Utility Namespace Containing Common Alignment Functions used within all allocators.
//------------

namespace PointerMath
{
	inline void* AlignForward(void* a_pAddr, uint8 a_alignment)
	{
		return reinterpret_cast<void*>
			(
				reinterpret_cast<uintptr>(a_pAddr)
				+
				static_cast<uintptr>(a_alignment) - 1 // Add 
				&
				static_cast<uintptr>(~(a_alignment - 1)) // Round Down to next multiple
			);
	}

	inline uint8 AlignForwardAdjustment(const void* a_pAddress, uint8 a_alignment)
	{
		const uint8 adjustment = a_alignment - (reinterpret_cast<uintptr>(a_pAddress)& static_cast<uintptr>(a_alignment - 1));

		if (adjustment == a_alignment) // already aligned
			return 0;

		return adjustment;
	}
}
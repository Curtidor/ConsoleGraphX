#pragma once
#include <cassert>

namespace ConsoleGraphX_Interal
{
#ifdef NDEBUG
	#define CGX_VERIFY(ptr) ((void)0)
#else NDEBUG
	#define CGX_VERIFY(ptr) assert((ptr) != nullptr)
#endif
}
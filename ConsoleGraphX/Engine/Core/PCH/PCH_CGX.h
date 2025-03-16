#pragma once

#pragma warning(push)
#pragma warning(disable : 4820)  // Disable VCIC002 warning for the whole file

// standard library headers (general utilities, memory management, algorithms, and types)
#include <iostream>
#include <memory>
#include <algorithm>
#include <functional>
#include <chrono>
#include <optional>
#include <stdexcept>
#include <utility>
#include <type_traits>
#include <filesystem>

// data structures 
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <queue>

// windows headers (system-specific headers)
#define WIN32_LEAN_AND_MEAN // exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <wincontypes.h>  // windows console types
#include <cctype>         // character handling functions

// common engine code
#include "Engine\Math\vector3.h"
#include "Engine\Math\vector2.h"
#include "Engine\Core\Utils\verify_macro.h"


#pragma warning(pop) 


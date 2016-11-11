// <copyright file="pch.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once

#ifdef _WIN32
#include <Windows.h>
#define WIN32_LEAN_AND_MEAN
#endif

// STL
#include <array>
#include <fstream>
#include <atomic>
#include <string>
#include <set>
#include <iterator>
#include <unordered_map>
#include <memory>
#include <thread>
#include <functional>
#include <mutex>
#include <vector>
#include <queue>
#include <iostream>
#include <ratio>
#include <future>

#include "PAL.Forward.h"

// Undef these pesky macros
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
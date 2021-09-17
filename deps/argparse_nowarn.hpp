#pragma once

#ifndef _MSVC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#else
#pragma warning( push, 0 )
#endif

#include "argparse.hpp"

#ifndef _MSVC
#pragma GCC diagnostic pop
#else
#pragma warning( pop )
#endif
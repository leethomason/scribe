#pragma once

#include <assert.h>
#include <fmt/core.h>

// REQUIRE is always on (debug & release) and will cause termination
#define REQUIRE(x)                                                 \
	if (!(x)) {	                                                \
		fmt::print("REQUIRE failed: '{}' {} in {}\n", #x, __LINE__, __FILE__);\
        assert(false);                                          \
		std::terminate();										\
	}															\

#pragma once

#include <assert.h>
#include <fmt/core.h>

#define REQUIRE(x)                                                 \
	if (!(x)) {	                                                \
		fmt::print("REQUIRE failed: '{}' {} in {}\n", #x, __LINE__, __FILE__);\
        assert(false);                                          \
		std::terminate();										\
	}															\

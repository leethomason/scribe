#pragma once

#define TEST(x)                                                 \
	if (!(x)) {	                                                \
		fmt::print("Test failure line {} in {}\n", __LINE__, __FILE__);\
        assert(false);                                          \
		std::terminate();										\
	}															\

#define TEST_FP(x, y)                                           \
	if (fabs(x - y) > 0.0001) {	                                \
		fmt::print("Test failure line {} in {}\n", __LINE__, __FILE__);\
        assert(false);                                          \
		std::terminate();										\
	}                                                           \

#define RUN_TEST(test) fmt::print("Test: {}\n", #test); test

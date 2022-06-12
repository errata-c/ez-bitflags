#include <iostream>

#include <catch2/catch_all.hpp>
#include <ez/BitFlags.hpp>

enum class Test {
	Zero,
	One,
	Two,
	Three,

	_Count,
	_EnableOperators, // Enable the bitwise operators for this enumeration.
	_None = 0,
};
using TestFlags = ez::BitFlags<Test>;

std::ostream& operator<<(std::ostream& os, Test val) {
	switch (val) {
	case Test::Zero:
		return os << "Zero";
	case Test::One:
		return os << "One";
	case Test::Two:
		return os << "Two";
	case Test::Three:
		return os << "Three";
	default: // no need to print _Count
		return os;
	}
}

constexpr TestFlags cflags0 = Test::Zero;
constexpr TestFlags cflags1 = Test::One | Test::Zero;
constexpr TestFlags cflags2 = cflags0 & TestFlags{ Test::One };

TEST_CASE("operators") {
	TestFlags flags = Test::Zero | Test::Two;

	REQUIRE(flags.contains(Test::Zero));

	REQUIRE(flags.anyOf(Test::Zero | Test::One));

	REQUIRE(!flags.allOf(Test::Zero | Test::One));

	flags = Test::Zero;
	REQUIRE(!flags.empty());
}

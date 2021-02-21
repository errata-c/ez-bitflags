#include <iostream>

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <ez/BitFlags.hpp>

enum class Test {
	Zero,
	One,
	Two,
	Three,
	_Count,
	_EnableOperators, // Enable the bitwise operators for this enumeration.
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

int main(int argc, char* argv[]) {
	TestFlags flags = Test::Zero | Test::Two;

	if (flags.contains(Test::Zero)) {
		fmt::print("contains\n");
	}
	if (flags.anyOf(Test::Zero | Test::One)) {
		fmt::print("anyOf\n");
	}
	if (flags.allOf(Test::Zero | Test::One)) {
		fmt::print("No go\n");
	}

	flags = Test::Zero;
	if (!flags.empty()) {
		fmt::print("Not empty\n");
	}
}

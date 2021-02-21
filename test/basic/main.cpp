#include <iostream>

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <ez/BitFlags.hpp>

enum class Test {
	Zero,
	One,
	Two,
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
	default: // no need to print _Count
		return os;
	}
}

enum Unscoped {
	Fun = 0,
	NotFun = 9999,
	_Count = 2,
};

// You are allowed to declare an invalid form of BitFlags, because the template is not actually evaluated until you
// use it
using UnscopedFlags = ez::BitFlags<Unscoped>;

enum class Sparse {
	Zero = 0,
	Two = 2,
	Four = 4,
	_Count
};

std::ostream& operator<<(std::ostream& os, Sparse value) {
	switch (value) {
	case Sparse::Zero:
		return os << "Zero";
	case Sparse::Two:
		return os << "Two";
	case Sparse::Four:
		return os << "Four";
	default:
		return os;
	}
}

using SparseFlags = ez::BitFlags<Sparse>;


namespace ez {
	enum class NSpace {
		Zero = 0,
		One = 1,
		Two = 2,
		_Count
	};
	using NSpaceFlags = ez::BitFlags<NSpace>;


	std::ostream& operator<<(std::ostream& os, ez::NSpace val) {
		switch (val) {
		case ez::NSpace::Zero:
			return os << "Zero";
		case ez::NSpace::One:
			return os << "One";
		case ez::NSpace::Two:
			return os << "Two";
		default:
			return os;
		}
	}
}
using ez::operator<<;

// This is required to make a ostream operator available to libfmt, as it doesn't do the correct lookup.
namespace ez {
	inline std::ostream& operator<<(std::ostream& os, ez::NSpaceFlags val) {
		return ::operator<<(os, val);
	}
}

static constexpr bool has_op = ez::intern::has_ostream_operator<ez::NSpace>::value;

int main() {
	fmt::print("Test\n");
	// Next line should not compile if uncommented.
	//UnscopedFlags badflags;

	TestFlags value = TestFlags::None;

	value |= Test::Zero;

	fmt::print("Test::Zero print result: '{}'\n", Test::Zero);
	fmt::print("Test::Zero in BitFlags print result: '{}'\n", value);

	value = Test::One | Test::Two;

	fmt::print("Test::One | Test::Two print result: '{}'\n", value);

	SparseFlags sparse;
	sparse |= Sparse::Two;
	
	fmt::print("Sparse::Two print result: '{}'\n", sparse);

	sparse |= Sparse::Four;

	fmt::print("Sparse::Two | Sparse::Four print result: '{}'\n", sparse);

	ez::NSpaceFlags nspace;
	nspace |= ez::NSpace::One;

	fmt::print("NSpace::One print result: '{}'\n", nspace);

	return 0;
}
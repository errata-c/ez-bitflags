#include <iostream>

#include <catch2/catch.hpp>
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

std::ostream& operator<<(std::ostream& os, Test val);

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
	_Count,
	_EnableOperators,
	All = 0b10101,
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
static_assert(has_op, "has_op returned invalid result");


TEST_CASE("printing") {
	TestFlags value = TestFlags::None;
	value |= Test::Zero;

	std::string result = fmt::format("{}", Test::Zero);
	REQUIRE(result == "Zero");
	
	result = fmt::format("{}", value);
	REQUIRE(result == "[Zero]");

	value = Test::One | Test::Two;
	result = fmt::format("{}", value);
	REQUIRE(result == "[One, Two]");
}

TEST_CASE("sparse flags") {
	SparseFlags sparse = Sparse::Two | Sparse::Four;
	
	REQUIRE(sparse.rawValue() == 0b10100);

	std::string result = fmt::format("{}", sparse);
	REQUIRE(result == "[Two, Four]");

	sparse = ~sparse;
	REQUIRE(sparse.rawValue() == 1);
}
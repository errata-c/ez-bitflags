#include <iostream>
#include <sstream>

#include <catch2/catch.hpp>
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

	_Count = 3,
	_EnableOperators,

	_None = 0,
	_All = 0b10101,
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

		_Count,
		_None = 0,
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


TEST_CASE("bitcounts") {
	uint64_t u64 = 0xFF00'FF00'FF00'FF00ull;
	REQUIRE(ez::intern::bitcount(u64) == 32);
}

TEST_CASE("printing") {
	TestFlags value = TestFlags::None;
	value |= Test::Zero;

	std::stringstream ss;
	ss << Test::Zero;

	std::string result = ss.str();
	REQUIRE(result == "Zero");

	ss.str("");
	ss.clear();
	ss << value;

	result = ss.str();
	REQUIRE(result == "[Zero]");

	value = Test::One | Test::Two;
	ss.str("");
	ss.clear();
	ss << value;

	result = ss.str();
	REQUIRE(result == "[One, Two]");
}

TEST_CASE("sparse flags") {
	SparseFlags sparse = Sparse::Two | Sparse::Four;
	SparseFlags all = SparseFlags::All;

	int count = 0;
	for (Sparse flag : all) {
		++count;
	}
	REQUIRE(all.numSet() == 3);
	REQUIRE(count == 3);

	REQUIRE(all.rawValue() == 0b10101);
	REQUIRE(sparse.rawValue() == 0b10100);

	std::stringstream ss;
	ss << sparse;

	std::string result = ss.str();
	REQUIRE(result == "[Two, Four]");

	sparse = ~sparse;
	REQUIRE(sparse.rawValue() == 1);
}
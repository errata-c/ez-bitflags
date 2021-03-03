#include <catch2/catch.hpp>

#include <fmt/core.h>
#include <ez/BitFlags.hpp>

namespace foo {
	enum class Bar {
		Zero = 0,
		One = 1,
		Two = 2,
		Three = 3,
		_Count,
		_EnableOperators
	};
	using Bars = ez::BitFlags<Bar>;
}

namespace sigma {
	using Bar = foo::Bar;
	using Bars = foo::Bars;
}

TEST_CASE("values") {
	foo::Bars bars;
	
	SECTION("default constructor") {
		REQUIRE(bars.rawValue() == 0);
		REQUIRE(bars == foo::Bars::None);
	}

	bars |= foo::Bar::One;

	SECTION("or equal") {
		REQUIRE(bars.rawValue() == (1 << 1));
	}

	bars |= foo::Bar::Two | foo::Bar::Three | foo::Bar::Zero;

	SECTION("multiple or") {
		REQUIRE(bars.rawValue() == (0b1111));
	}
}

TEST_CASE("iteration") {
	sigma::Bars bars = sigma::Bar::One | sigma::Bar::Three;
	sigma::Bars inv = ~bars;

	SECTION("positive values") {
		sigma::Bar compare[] = {sigma::Bar::One, sigma::Bar::Three};
		int i = 0;
		for (sigma::Bar value: bars) {
			REQUIRE(compare[i] == value);
			++i;
		}
	}

	SECTION("inverse values") {
		sigma::Bar compare[] = { sigma::Bar::Zero, sigma::Bar::Two };
		int i = 0;
		for (sigma::Bar value : inv) {
			REQUIRE(compare[i] == value);
			++i;
		}
	}
}
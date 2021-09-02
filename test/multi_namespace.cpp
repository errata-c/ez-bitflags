#include <catch2/catch.hpp>
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
	SECTION("default constructor") {
		foo::Bars bars;

		REQUIRE(bars.rawValue() == 0);
		REQUIRE(bars == foo::Bars::None);
	}

	SECTION("or equal") {
		foo::Bars bars;

		bars |= foo::Bar::One;

		REQUIRE(bars.contains(foo::Bar::One));
		REQUIRE(bars.rawValue() == (1 << 1));
	}

	SECTION("multiple or") {
		foo::Bars bars;

		bars |= foo::Bar::One;
		bars |= foo::Bar::Two | foo::Bar::Three | foo::Bar::Zero;
		REQUIRE(bars.rawValue() == (0b1111));
	}

	SECTION("set method") {
		foo::Bars bars;

		bars = foo::Bars::None;
		bars.set(foo::Bar::One);

		REQUIRE(bars.contains(foo::Bar::One));
		REQUIRE(bars.rawValue() == 2);
	}

	SECTION("All value") {
		foo::Bars bars = foo::Bars::All;
		REQUIRE(bars.rawValue() == 0b1111);
	}

	SECTION("all to none") {
		foo::Bars bars = foo::Bars::All;

		REQUIRE(bars.rawValue() == 0b1111);

		bars = ~bars;
		REQUIRE(bars.rawValue() == 0);
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
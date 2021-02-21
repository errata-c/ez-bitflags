#pragma once
#include <cinttypes>
#include <type_traits>
#include <sstream>

namespace ez {
	namespace intern {
		template<typename Enum, bool B = std::is_enum_v<Enum>>
		struct is_scoped_enum : std::false_type {};
		template<typename Enum>
		struct is_scoped_enum<Enum, true> {
			static constexpr bool value = !std::is_convertible_v<Enum, std::underlying_type_t<Enum>>;
		};
		
		template<typename Enum, typename = int>
		struct has_count: std::false_type {};
		template<typename Enum>
		struct has_count<Enum, decltype(Enum::_Count, 0)>: std::true_type {};

		template<typename Enum, typename = int>
		struct has_ostream_operator: std::false_type {};
		template<typename Enum>
		struct has_ostream_operator<Enum, decltype(std::declval<std::ostream>() << Enum::_Count, 0)> : std::true_type {};

		template<typename Enum>
		constexpr bool verify() {
			static_assert(has_count<Enum>::value, "Your enumeration does not have a _Count declared!");
			return static_cast<std::size_t>(Enum::_Count) <= static_cast<std::size_t>(64);
		}

		template<typename utype, std::size_t _Count>
		constexpr utype bitmask() {
			constexpr utype one = static_cast<utype>(1);
			constexpr std::size_t size = sizeof(utype) * 8;
			constexpr std::size_t complement = size - _Count;

			return (~static_cast<utype>(0)) >> complement;
		}

		template<typename Enum>
		struct EnumTraits {
			static_assert(is_scoped_enum<Enum>::value, "Type provided to ez::BitFlags must be a scoped enumeration.");
			static_assert(has_count<Enum>::value, "Enumeration provided to ez::BitFlags must have a _Count enumerator, describing how many flags there are.");
			
			using utype = std::make_unsigned_t<std::underlying_type_t<Enum>>;
			
			static_assert(static_cast<utype>(Enum::_Count) <= static_cast<utype>(64), 
				"The enumeration provided to ez::BitFlags either has too many members, or the _Count is incorrect.");
			
			static constexpr utype mask = bitmask<utype, static_cast<std::size_t>(Enum::_Count)>();
		};
	};

	template<typename Enum>
	struct BitFlags {
		using traits = intern::EnumTraits<Enum>;
		using utype = typename traits::utype;

		struct none_t {};
		struct all_t {};

		static constexpr none_t None{};
		static constexpr all_t All{};

		BitFlags() noexcept
			: value(0)
		{}
		BitFlags(none_t none) noexcept
			: value(0)
		{}
		BitFlags(all_t all) noexcept
			: value(traits::mask)
		{}

		BitFlags(const BitFlags&) noexcept = default;
		BitFlags& operator=(const BitFlags&) noexcept = default;

		BitFlags(const Enum& val) noexcept
			: value(_convert(val))
		{}
		BitFlags& operator=(const Enum& val) noexcept {
			value = _convert(val);
			return *this;
		}

		bool contains(Enum val) const noexcept {
			return (value & _convert(val)) != static_cast<utype>(0);
		}
		bool allOf(BitFlags other) const noexcept {
			return (value & other.value) == other.value;
		}
		bool noneOf(BitFlags other) const noexcept {
			return (value & other.value) == static_cast<utype>(0);
		}
		bool anyOf(BitFlags other) const noexcept {
			return (value & other.value) != static_cast<utype>(0);
		}

		BitFlags& operator|=(BitFlags rh) noexcept {
			value |= rh.value;
			return *this;
		}

		BitFlags& operator&=(BitFlags rh) noexcept {
			value &= rh.value;
			return *this;
		}

		BitFlags& operator^=(BitFlags rh) noexcept {
			value ^= rh.value;
			value &= traits::mask;
			return *this;
		}

		bool operator==(BitFlags other) const noexcept {
			return value == other.value;
		}
		bool operator!=(BitFlags other) const noexcept {
			return value != other.value;
		}

		utype rawValue() const noexcept {
			return value;
		}
	private:
		utype value;
		static constexpr utype _convert(Enum val) {
			return static_cast<utype>(1) << static_cast<utype>(val);
		}
	};
};

template<typename Enum>
ez::BitFlags<Enum>(operator|)(ez::BitFlags<Enum> lh, ez::BitFlags<Enum> rh) noexcept {
	lh |= rh;
	return lh;
}
template<typename Enum>
ez::BitFlags<Enum>(operator&)(ez::BitFlags<Enum> lh, ez::BitFlags<Enum> rh) noexcept {
	lh &= rh;
	return lh;
}
template<typename Enum>
ez::BitFlags<Enum>(operator^)(ez::BitFlags<Enum> lh, ez::BitFlags<Enum> rh) noexcept {
	lh ^= rh;
	return lh;
}
template<typename Enum>
ez::BitFlags<Enum>(operator~)(ez::BitFlags<Enum> lh) noexcept {
	lh ^= ez::BitFlags<Enum>::All;
	return lh;
}

template<typename Enum, Enum = Enum::_EnableOperators>
ez::BitFlags<Enum>(operator|)(Enum lh, Enum rh) noexcept {
	ez::BitFlags<Enum> tmp{lh};
	tmp |= rh;
	return tmp;
}
template<typename Enum, Enum = Enum::_EnableOperators>
ez::BitFlags<Enum>(operator&)(Enum lh, Enum rh) noexcept {
	ez::BitFlags<Enum> tmp{ lh };
	tmp &= rh;
	return tmp;
}
template<typename Enum, Enum = Enum::_EnableOperators>
ez::BitFlags<Enum>(operator^)(Enum lh, Enum rh) noexcept {
	ez::BitFlags<Enum> tmp{ lh };
	tmp ^= rh;
	return tmp;
}
template<typename Enum, Enum = Enum::_EnableOperators>
ez::BitFlags<Enum>(operator~)(Enum lh) noexcept {
	ez::BitFlags<Enum> tmp{ lh };
	return ~tmp;
}

template<typename Enum, typename = std::enable_if_t<ez::intern::has_ostream_operator<Enum>::value>>
std::ostream& operator<<(std::ostream & os, ez::BitFlags<Enum> val) {
	using flags_t = ez::BitFlags<Enum>;
	using traits = typename flags_t::traits;
	using utype = typename traits::utype;
	constexpr utype maxval = static_cast<utype>(Enum::_Count);

	// String stream is required to support printing of sparse bitfields. 
	// tellp() actually returns useful information on stringstream classes.
	std::stringstream ss;

	ss.put('[');
	for (utype i = 0; i < maxval; ++i) {
		if (val.contains(static_cast<Enum>(i))) {

			std::streampos loc = ss.tellp();
			ss << static_cast<Enum>(i);
			if (loc != os.tellp()) { // Actually wrote something
				ss.write(", ", 2);
			}
		}
	}
	
	std::string str = ss.str();
	if (str.size() >= 3) {
		// Get rid of trailing comma if it exists
		str.pop_back();
		str.pop_back();
	}
	str.push_back(']');

	os << str;

	return os;
}
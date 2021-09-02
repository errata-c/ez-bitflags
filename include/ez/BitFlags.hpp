#pragma once
#include <cinttypes>
#include <cassert>
#include <type_traits>
#include <ostream>

namespace ez {
	namespace intern {
		static constexpr uint64_t const_bitcount_4(uint64_t val) {
			return ((val & 0xFFFFFFFF'00000000ull) >> 32) + (val & 0x00000000'FFFFFFFFull);
		}
		static constexpr uint64_t const_bitcount_3(uint64_t val) {
			return const_bitcount_4(((val & 0xFFFF0000'FFFF0000ull) >> 16) + (val & 0x0000FFFF'0000FFFFull));
		}
		static constexpr uint64_t const_bitcount_2(uint64_t val) {
			return const_bitcount_3(((val & 0xFF00'FF00'FF00'FF00ull) >> 8) + (val & 0x00FF'00FF'00FF00FFull));
		}
		static constexpr uint64_t const_bitcount_1(uint64_t val) {
			return const_bitcount_2(((val & 0xF0F0'F0F0'F0F0'F0F0ull) >> 4) + (val & 0x0F0F'0F0F'0F0F'0F0Full));
		}
		static constexpr uint64_t const_bitcount_0(uint64_t val) {
			return const_bitcount_1(((val & 0xCCCC'CCCC'CCCC'CCCCull) >> 2) + (val & 0x3333'3333'3333'3333ull));
		}
		static constexpr std::size_t const_bitcount(uint64_t val) {
			return const_bitcount_0(((val & 0xAAAA'AAAA'AAAA'AAAAull) >> 1) + (val & 0x5555'5555'5555'5555ull));
		}
		
		static constexpr int last_set(uint64_t val) {
			return val == 0 ? 0 : 1 + last_set(val >> 1);
		}

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
		struct has_improper_all : std::false_type {};
		template<typename Enum>
		struct has_improper_all<Enum, decltype(Enum::All, 0)> : std::true_type {};

		template<typename Enum, typename = int>
		struct has_improper_none : std::false_type {};
		template<typename Enum>
		struct has_improper_none<Enum, decltype(Enum::None, 0)> : std::true_type {};

		template<typename Enum, typename = int>
		struct has_ostream_operator: std::false_type {};
		template<typename Enum>
		struct has_ostream_operator<Enum, decltype(operator<<(std::declval<std::ostream>(), Enum::_Count), 0)> : std::true_type {};

		template<typename Enum, typename = int>
		struct all_value {
			using utype = std::make_unsigned_t<std::underlying_type_t<Enum>>;
			static constexpr utype value = ~((~utype(1)) << (static_cast<int>(Enum::_Count) - 1));
		};
		template<typename Enum>
		struct all_value<Enum, decltype(Enum::_All, 0)> {
			using utype = std::make_unsigned_t<std::underlying_type_t<Enum>>;
			static constexpr utype value = static_cast<utype>(Enum::_All);
		};

		template<typename Enum, typename = int>
		struct none_value {
			using utype = std::make_unsigned_t<std::underlying_type_t<Enum>>;
			static constexpr utype value = 0;
		};
		template<typename Enum>
		struct none_value<Enum, decltype(Enum::_None, 0)> {
			using utype = std::make_unsigned_t<std::underlying_type_t<Enum>>;
			static constexpr utype value = static_cast<utype>(Enum::_None);
		};

		template<typename Enum>
		struct EnumTraits {
			static_assert(is_scoped_enum<Enum>::value, 
				"Type provided to ez::BitFlags must be a scoped enumeration.");

			static_assert(has_count<Enum>::value, 
				"Enumeration provided to ez::BitFlags must have a _Count enumerator, describing how many flags there are.");

			static_assert(!has_improper_all<Enum>::value,
				"Enumeration provided to ez::BitFlags contains an enumerator named 'All' instead of '_All'");

			static_assert(!has_improper_none<Enum>::value,
				"Enumeration provided to ez::BitFlags contains an enumerator named 'None' instead of '_None'");

			using utype = std::make_unsigned_t<std::underlying_type_t<Enum>>;
			
			static_assert(static_cast<utype>(Enum::_Count) <= static_cast<utype>(64), 
				"The enumeration provided to ez::BitFlags either has too many members, or the _Count is incorrect.");

			static constexpr utype All = all_value<Enum>::value;
			static constexpr utype None = none_value<Enum>::value;
			static constexpr utype Last = last_set(All);

			static_assert(static_cast<utype>(Enum::_Count) == const_bitcount(All), 
				"The enumerator _All does not match the number of flags specified in _Count.");
		};

		// Simple functions to perform popcount, ie number of 1s
		static std::size_t bitcount(uint8_t _val) noexcept {
			uint32_t val = _val;
			val = ((val & 0xAAu) >> 1) + (val & 0x55u);
			val = ((val & 0xCCu) >> 2) + (val & 0x33u);
			val = ((val & 0xF0) >> 4) + (val & 0x0F);
			return static_cast<std::size_t>(val);
		}
		static std::size_t bitcount(uint16_t _val) noexcept {
			uint32_t val = _val;
			val = ((val & 0xAAAAu) >> 1) + (val & 0x5555u);
			val = ((val & 0xCCCCu) >> 2) + (val & 0x3333u);
			val = ((val & 0xF0F0u) >> 4) + (val & 0x0F0Fu);
			val = ((val & 0xFF00u) >> 8) + (val & 0x00FFu);

			return static_cast<std::size_t>(val);
		}
		static std::size_t bitcount(uint32_t val) noexcept {
			val = ((val & 0xAAAA'AAAAu) >> 1) + (val & 0x5555'5555u);
			val = ((val & 0xCCCC'CCCCu) >> 2) + (val & 0x3333'3333u);
			val = ((val & 0xF0F0'F0F0u) >> 4) + (val & 0x0F0F'0F0Fu);
			val = ((val & 0xFF00'FF00u) >> 8) + (val & 0x00FF'00FFu);
			val = ((val & 0xFFFF'0000u) >> 16) + (val & 0x0000'FFFFu);

			return static_cast<std::size_t>(val);
		}
		static std::size_t bitcount(uint64_t val) noexcept {
			val = ((val & 0xAAAA'AAAA'AAAA'AAAAull) >> 1) + (val & 0x5555'5555'5555'5555ull);
			val = ((val & 0xCCCC'CCCC'CCCC'CCCCull) >> 2) + (val & 0x3333'3333'3333'3333ull);
			val = ((val & 0xF0F0'F0F0'F0F0'F0F0ull) >> 4) + (val & 0x0F0F'0F0F'0F0F'0F0Full);
			val = ((val & 0xFF00'FF00'FF00'FF00ull) >> 8) + (val & 0x00FF'00FF'00FF00FFull);
			val = ((val & 0xFFFF0000'FFFF0000ull) >> 16) + (val & 0x0000FFFF'0000FFFFull);
			val = ((val & 0xFFFFFFFF'00000000ull) >> 32) + (val & 0x00000000'FFFFFFFFull);

			return static_cast<std::size_t>(val);
		}
	};

	template<typename Enum>
	struct BitFlags {
		using traits = typename intern::EnumTraits<Enum>;
		using utype = typename traits::utype;

		struct none_t {};
		struct all_t {};

		// Custom types for None and All to make sure they are handled correctly
		static constexpr none_t None{};
		static constexpr all_t All{};

		struct const_iterator {
			static constexpr utype maxval = traits::Last;

			using value_type = Enum;
			using difference_type = int;
			using pointer = const Enum*;
			using reference = const Enum&;
			using iterator_category = std::bidirectional_iterator_tag;

			const_iterator(const BitFlags& _source, int _index) 
				: source(&_source)
				, index(_index)
			{}

			const_iterator() noexcept
				: index(0)
				, source(nullptr)
			{};

			const_iterator(const const_iterator &) noexcept = default;
			~const_iterator() = default;
			const_iterator& operator=(const const_iterator&) noexcept = default;

			bool operator==(const_iterator other) const noexcept {
				return index == other.index;
			}
			bool operator!=(const_iterator other) const noexcept {
				return index != other.index;
			}

			value_type operator*() noexcept {
				return static_cast<Enum>(index);
			}
			value_type operator->() noexcept {
				return static_cast<Enum>(index);
			}

			const_iterator operator++(int) noexcept {
				const_iterator copy = *this;
				++(*this);
				return copy;
			}
			const_iterator operator--(int) noexcept {
				const_iterator copy = *this;
				--(*this);
				return copy;
			}

			const_iterator& operator++() noexcept {
				assert(source != nullptr);
				++index;
				while (index < maxval && !source->contains(static_cast<Enum>(index))) {
					++index;
				}
				return *this;
			}
			const_iterator& operator--() noexcept {
				assert(source != nullptr);
				--index;
				while (index > -1 && !source->contains(static_cast<Enum>(index))) {
					--index;
				}
				return *this;
			}
		private:
			int index;
			const BitFlags* source;
		};

		static constexpr BitFlags fromRawValue(utype val) {
			BitFlags flags;
			flags.value = val;
			return flags;
		}

		constexpr BitFlags() noexcept
			: value(intern::none_value<Enum>::value)
		{}
		constexpr BitFlags(none_t none) noexcept
			: value(intern::none_value<Enum>::value)
		{}
		constexpr BitFlags(all_t all) noexcept
			: value(intern::all_value<Enum>::value)
		{}

		constexpr BitFlags(const Enum& val) noexcept
			: value(_convert(val))
		{}
		constexpr BitFlags& operator=(const Enum& val) noexcept {
			value = _convert(val);
			return *this;
		}

		constexpr BitFlags(const BitFlags&) noexcept = default;
		constexpr BitFlags& operator=(const BitFlags&) noexcept = default;

		void clear() noexcept {
			value = static_cast<utype>(0);
		}
		constexpr bool empty() const noexcept {
			return value == static_cast<utype>(0);
		}
		std::size_t size() const noexcept {
			return intern::bitcount(value);
		}
		constexpr std::size_t max_size() const noexcept {
			return static_cast<std::size_t>(Enum::_Count);
		}

		constexpr std::size_t numSet() const noexcept {
			return size();
		}
		constexpr std::size_t numUnset() const noexcept {
			return max_size() - size();
		}

		void set(BitFlags val) noexcept {
			*this |= val;
		}
		void unset(BitFlags val) noexcept {
			*this &= ~val;
		}

		const Enum front() const {
			assert(!empty());
			return *begin();
		}
		const Enum back() const {
			assert(!empty());
			return *(--end());
		}

		constexpr bool contains(Enum val) const noexcept {
			return (value & _convert(val)) != static_cast<utype>(0);
		}
		constexpr bool allOf(BitFlags other) const noexcept {
			return (value & other.value) == other.value;
		}
		constexpr bool noneOf(BitFlags other) const noexcept {
			return (value & other.value) == static_cast<utype>(0);
		}
		constexpr bool anyOf(BitFlags other) const noexcept {
			return (value & other.value) != static_cast<utype>(0);
		}

		constexpr BitFlags& operator|=(BitFlags rh) noexcept {
			value |= rh.value;
			return *this;
		}

		constexpr BitFlags& operator&=(BitFlags rh) noexcept {
			value &= rh.value;
			return *this;
		}

		constexpr BitFlags& operator^=(BitFlags rh) noexcept {
			value ^= rh.value;
			value &= traits::mask;
			return *this;
		}

		constexpr bool operator==(BitFlags other) const noexcept {
			return value == other.value;
		}
		constexpr bool operator!=(BitFlags other) const noexcept {
			return value != other.value;
		}

		constexpr utype rawValue() const noexcept {
			return value;
		}

		const_iterator begin() const noexcept {
			auto tmp = const_iterator{ *this, -1 };
			return ++tmp; // No guarantee that there are any set, increment once to find first, if it exists. 
		}
		const_iterator end() const noexcept {
			return const_iterator{ *this, const_iterator::maxval };
		}

		const_iterator cbegin() const noexcept {
			auto tmp = const_iterator{ *this, -1 };
			return ++tmp; // No guarantee that there are any set, increment once to find first, if it exists. 
		}
		const_iterator cend() const noexcept {
			return const_iterator{ *this, const_iterator::maxval };
		}
	private:
		utype value;
		static constexpr utype _convert(Enum val) {
			return static_cast<utype>(1) << static_cast<utype>(val);
		}
	};
};

template<typename Enum>
constexpr ez::BitFlags<Enum>(operator|)(ez::BitFlags<Enum> lh, ez::BitFlags<Enum> rh) noexcept {
	return ez::BitFlags<Enum>::fromRawValue(lh.rawValue() | rh.rawValue());
}
template<typename Enum>
constexpr ez::BitFlags<Enum>(operator&)(ez::BitFlags<Enum> lh, ez::BitFlags<Enum> rh) noexcept {
	return ez::BitFlags<Enum>::fromRawValue(lh.rawValue() & rh.rawValue());
}
template<typename Enum>
constexpr ez::BitFlags<Enum>(operator^)(ez::BitFlags<Enum> lh, ez::BitFlags<Enum> rh) noexcept {
	return ez::BitFlags<Enum>::fromRawValue(lh.rawValue() ^ rh.rawValue()) & ez::BitFlags<Enum>{ez::BitFlags<Enum>::All};
}
template<typename Enum>
constexpr ez::BitFlags<Enum>(operator~)(ez::BitFlags<Enum> lh) noexcept {
	return ez::BitFlags<Enum>::fromRawValue(~lh.rawValue()) & ez::BitFlags<Enum>{ez::BitFlags<Enum>::All};
}

template<typename Enum>
constexpr ez::BitFlags<Enum>(operator|)(ez::BitFlags<Enum> lh, Enum rh) noexcept {
	return lh | ez::BitFlags<Enum>{rh};
}
template<typename Enum>
constexpr ez::BitFlags<Enum>(operator&)(ez::BitFlags<Enum> lh, Enum rh) noexcept {
	return lh | ez::BitFlags<Enum>{rh};
}
template<typename Enum>
constexpr ez::BitFlags<Enum>(operator^)(ez::BitFlags<Enum> lh, Enum rh) noexcept {
	return lh ^ ez::BitFlags<Enum>{rh};
}

template<typename Enum, Enum = Enum::_EnableOperators>
constexpr ez::BitFlags<Enum>(operator|)(Enum lh, Enum rh) noexcept {
	return ez::BitFlags<Enum>{lh} | ez::BitFlags<Enum>{rh};
}
template<typename Enum, Enum = Enum::_EnableOperators>
constexpr ez::BitFlags<Enum>(operator&)(Enum lh, Enum rh) noexcept {
	return ez::BitFlags<Enum>{lh} & ez::BitFlags<Enum>{rh};
}
template<typename Enum, Enum = Enum::_EnableOperators>
constexpr ez::BitFlags<Enum>(operator^)(Enum lh, Enum rh) noexcept {
	return ez::BitFlags<Enum>{lh} ^ ez::BitFlags<Enum>{rh};
}
template<typename Enum, Enum = Enum::_EnableOperators>
constexpr ez::BitFlags<Enum>(operator~)(Enum lh) noexcept {
	return ~ez::BitFlags<Enum>{ lh };
}

template<typename Enum, typename = std::enable_if_t<ez::intern::has_ostream_operator<Enum>::value>>
std::ostream& operator<<(std::ostream & os, ez::BitFlags<Enum> val) {
	using flags_t = ez::BitFlags<Enum>;
	using traits = typename flags_t::traits;
	using utype = typename traits::utype;
	constexpr utype maxval = static_cast<utype>(Enum::_Count);

	ez::BitFlags<Enum> all = ez::BitFlags<Enum>::All;
	
	os.put('[');
	for (auto it = val.begin(), last = val.end(); it != last; ++it) {
		Enum flag = *it;
		if (all.contains(flag)) {
			os << flag;
			auto copy = it;
			++copy;
			if (copy != last) { // Not the last flag
				os.write(", ", 2);
			}
		}
	}
	os.put(']');

	return os;
}
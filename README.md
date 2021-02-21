# ez-bitflags

A simple single header file bit flags implementation.
Takes a scoped enumeration as template value.
The enumeration must have a `_Count` value defined, with the number of enumerators in it.

Additionally an `_EnableOperators` value can be defined within the enumeration, which will enable the bitwise operators for the enumeration type, each of which returns the coresponding bit flags type.

Finally, if an overload of `ostream operator<<` is provided for the base enumeration, the bit flag class will have its own implementation of the operator enabled as well. The operator will print the values present in the bit field in ascending order like so: `"[One, Two, Three]"`

## Example
```cpp
enum class Enum {
	Zero = 0,
	One = 1,
	Two = 2,
	_Count,
	_EnableOperators
}; 

using EnumFlags = ez::BitFlags<Enum>;

EnumFlags defaultFlags() {
	return Enum::Zero | Enum::Two;
}
```
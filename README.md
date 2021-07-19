# ez-bitflags

A simple single header file bit flags implementation.
Takes a scoped enumeration as template value.
The enumeration must have a `_Count` value defined, with the number of enumerators in it.

Additionally an `_EnableOperators` value can be defined within the enumeration, which will enable the bitwise operators for the enumeration type, each of which returns the corresponding bit flags type.
An `_All` and `_None` enumerator can defined as well to specify what the default none and all values should be for the bitflags.

Finally, if an overload of `ostream operator<<` is provided for the base enumeration, the bit flag class will have its own implementation of the operator enabled as well. The operator will print the values present in the bit field in ascending order like so: `"[One, Two, Three]"`

## Example
```cpp
#include <ez/BitFlags.hpp>

enum class Sparse {
	Zero = 0,
	Two = 2,
	Four = 4,
	_Count,
	_EnableOperators,
	_All = 0b10101, // Since these are sparse (gaps in between enumerators) we define an _All
}; 

using SparseFlags = ez::BitFlags<Sparse>;

SparseFlags defaultFlags() {
	return Sparse::Zero | Sparse::Two;
}
```
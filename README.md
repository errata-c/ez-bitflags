# ez-bitflags

A simple single header file bit flags implementation.

The enumeration must be a scoped enumeration type (enum class) and must have a `_Count` value defined, specifying the number of enumerators.

An `_EnableOperators` value can be defined within the enumeration, which will enable the bitwise operators for the enumeration type, each of which returns the corresponding bit flags type.
A `_None` enumerator can be defined as well to specify what the default none value should be for the bitflags.

An ```_All``` enumerator can be defined to specify what the set of all flags combined will look like, so that sparse flags can be supported. (See Example section for more info on what sparse flags look like)

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
	_All = 0b10101, // Since these are sparse (gaps in between enumerators) we define an _All value
}; 

using SparseFlags = ez::BitFlags<Sparse>;

SparseFlags defaultFlags() {
    // _EnableOperators allows the bitwise operators to be used as expected.
	return Sparse::Zero | Sparse::Two;
}
```


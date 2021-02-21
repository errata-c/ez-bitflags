## ez-bitflags

A simple single header file bit flags implementation.
Takes a scoped enumeration as template value.
The enumeration must have a `_Count` value defined, with the number of enumerators in it.
Additionally a `_EnableOperators` value can be defined within the enumeration, which will enable
the bitwise operators for the enumeration type, each of with returns the coresponding bit flags type.
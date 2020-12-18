# float16_t

`float16_t` is a header only c++ library for [half-precision floating-point format](https://en.wikipedia.org/wiki/Half-precision_floating-point_format).


----

## Usage:

**include the header file**

```cpp
#include "float16_t.hpp"
using numeric::float16_t;
```

then compile using a C++20 compatible compiler with command line like:
```
clang++ -c -std=c++2a -Wall -Wextra -ferror-limit=1 -ftemplate-backtrace-limit=0 -funsafe-math-optimizations  -Ofast -flto -pipe -march=native -DDEBUG -o ./obj/test_test.o tests/test.cc
```

if you do not want to dump binary information, you can ommit the `-DDEBUG` option.


Example code:

```cpp
    float16_t f = 1.1f;
    {
        std::cout << "f =" << f << std::endl;
        std::cout << "float(f) =" << float(f) << std::endl;
        std::cout << "f++ =" << f++ << std::endl;
        std::cout << "++f =" << ++f << std::endl;
        std::cout << "f-- =" << f-- << std::endl;
        std::cout << "--f =" << --f << std::endl;
    }

```
this produces

```
f =1.09961(0 01111 0001100110)
float(f) =1.09961
f++ =1.09961(0 01111 0001100110)
++f =3.09961(0 10000 1000110011)
f-- =3.09961(0 10000 1000110011)
--f =1.09961(0 01111 0001100110)
```

And

```cpp
    float16_t g = -1.3f;
    {
        std::cout << "f+g = " << f+g << std::endl;
        std::cout << "f-g = " << f-g << std::endl;
        std::cout << "f*g = " << f*g << std::endl;
        std::cout << "f/g = " << f/g << std::endl;
    }

```
produces

```
f+g = -0.200195(1 01100 1001101000)
f-g = 2.39844(0 10000 0011001100)
f*g = -1.42969(1 01111 0110111000)
f/g = -0.846191(1 01110 1011000101)
```

Moreover,

```cpp
    float16_t h = 0.27f;
    {
        std::cout << "numeric::sin(h) = " << numeric::sin(h) << std::endl;
        std::cout << "numeric::cos(h) = " << numeric::cos(h) << std::endl;
        std::cout << "numeric::sqrt(h) = " << numeric::sqrt(h) << std::endl;
        std::cout << "numeric::cbrt(h) = " << numeric::cbrt(h) << std::endl;
    }

```

produces

```
numeric::sin(h) = 0.266846(0 01101 0001000101)
numeric::cos(h) = 0.963867(0 01110 1110110110)
numeric::sqrt(h) = 0.519531(0 01110 0000101000)
numeric::cbrt(h) = 0.646484(0 01110 0100101100)

```

For more information, please check the source file `float16_t.hpp`.


## Acknowledgements:

+ [half float](https://github.com/acgessler/half_float)
+ [float16](https://github.com/x448/float16)

## License

BSD License, Anti-996 License.


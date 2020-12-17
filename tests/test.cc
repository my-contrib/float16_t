#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "../float16_t.hpp"
//#include "../half.hpp"
#include <cmath>
#include <iostream>
#include <bitset>

void print( float x )
{
    using float16_private::float32_to_float16;
    auto [sign, exponent, fraction] = float32_to_float16( x );
    std::cout.precision( 15 );
    std::cout << x << ":\t" << std::bitset<1>(sign) << " " << std::bitset<5>(exponent) << " " << std::bitset<10>(fraction) << std::endl;
}

TEST_CASE( "32_2_16", "[32_2_16]" )
{
    print( 0.000000059604645 );
    print( 0.000000059604647 );
    print( 0.0 );
    print( -0.000000059604645 );
    print( -0.000000059604644 );
    print( 0.000060975552f  );
    print( -0.000060975552f  );

    print( 0.00006103515625  );
    print( -0.00006103515625  );

    print( 65504.0f  );
    print( -65504.0f  );
    print( 65505.0f  );
    print( -65505.0f  );

    print( 0.999511720f  );
    print( -0.999511720f  );

    print( 1.0f  );
    print( -1.0f  );

    print(  1.00097656f  );
    print(  -1.00097656f  );

    print(  -1.0f/0.0f  );

    print( 16504.0f  );
    print( -16204.0f  );
}

/*
TEST_CASE( "f1", "[f1]" )
{
    float const mx = 6504.0;
    float const interval = mx / 100000.0;
    float x = -mx;
    for ( ; x < mx; x += interval )
    {
        float16_t const f_{ x };
        float const x_ = float( f_ );
        REQUIRE( std::abs(x- x_) < 1.0f );
    }
}
*/

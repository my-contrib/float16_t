#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "../float16_t.hpp"
#include <cmath>
#include <iostream>
#include <bitset>

void print( float x )
{
    using numeric::float16_t_private::float32_to_float16;
    using numeric::float16_t_private::float16_to_float32;

    auto f16 = float32_to_float16( x );
    std::cout << x << " => " << f16 << std::endl;

    auto f32 = float16_to_float32( f16.bits_ );
    std::cout << "f32 => " << f32 << std::endl;

    //_Float16 _f16 = x;
    //float _x = _f16 * 1.0f;
    //std::cout << _x << std::endl;

    std::cout << "****************************" << std::endl;
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

    print( std::numeric_limits<float>::infinity() );
    print( std::numeric_limits<float>::quiet_NaN() );

    print( 16504.0f  );
    print( -16204.0f  );

    for ( float x = -1.0f; x <= 1.0f; x+= 0.013 )
    {
        numeric::float16_t f{x};
        std::cout << x << " : " << f << std::endl;
    }
}

TEST_CASE( "f1", "[f1]" )
{
    float const mx = 10.0;
    float const interval = mx / 1000.0;
    float x = -mx;
    for ( ; x < mx; x += interval )
    {
        numeric::float16_t const f_{ x };
        float const x_ = float( f_ );
        REQUIRE( std::abs(x- x_) < 0.01f );
    }
}

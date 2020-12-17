#include <cstdint>
#include <algorithm>
#include <limits>
#include <iostream>
#include <tuple>
#include <cmath>

namespace float16_private
{

    /*
    Exponent	        Significand = zero	    Significand ≠ zero	    Equation
    00000	            zero, −0	            subnormal numbers	    (−1)signbit × 2^−14 × 0.significantbits2
    00001, ..., 11110	normalized value	                            (−1)signbit × 2^(exponent−15) × 1.significantbits2
    11111	            ±infinity	            NaN (quiet, signalling)
    */
    // returns <sign, exponent, fraction> -- int for convinience
    auto const float32_to_float16 = []( float other )
    {
        int const sign = other > 0.0 ? 0 : 1; // sign

        // NAN
        if ( std::isnan( other ) )
            return std::make_tuple( sign, 31, 1 );

        // infinity
        if ( std::isinf( other ) )
            return std::make_tuple( sign, 31, 0 );

        other = other < 0.0 ? -other : other; // positive it

        // infinity again
        if ( other > 65504.0f  )
            return std::make_tuple( sign, 31, 0 );

        // 0 for very small values
        if ( other < 0.00000005960464477539f )
            return std::make_tuple( 0, 0, 0 );

        // sub normal
        if ( other < 0.00006097555160522461f )
        {
            int const exponent = 0;
            int const fraction = static_cast<int>(std::round(other * 16777216.0f));
            return std::make_tuple( sign, exponent, fraction );
        }

        float const lg = std::log2( other * 32768.0f );
        int const exponent = static_cast<int>( std::floor( lg ) );
        int const fraction = static_cast<int>(std::round( std::pow( 2.0f, lg-exponent ) * 1024.0f ));
        return std::make_tuple( sign, exponent, fraction );
    };



}//float16_private

struct float16
{
    union
    {
        std::uint16_t bits;
        struct
        {
            std::uint16_t frac_ : 10;
            std::uint16_t exp_ : 5;
            std::uint16_t sign_ : 1;
        } IEEE;
    };

    constexpr float16() noexcept = default;
    constexpr float16( float16 const& ) noexcept = default;
    constexpr float16( float16&& ) noexcept = default;
    float16( float other )
    {
        //...
    }

    constexpr float16& operator = ( float16 const& ) noexcept = default;
    constexpr float16& operator = ( float16&& ) noexcept = default;
    float16& operator = ( float other )
    {
        float16 f{ other };
        std::swap( *this, f );
        return *this;
    }

    operator float() const
    {
        //
        return 0.0f;
    }

};

using float16_t = float16;

inline std::ostream& operator<< ( std::ostream& os, float16_t const& f )
{
    os  << "Frac: " << f.IEEE.frac_
        << " Exp: "  << f.IEEE.exp_
        << " Sign: " << f.IEEE.sign_;
    return os << " --> " << float( f );
}


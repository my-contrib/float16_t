#include <cstdint>
#include <limits>
#include <iostream>
#include <cmath>
#include <bitset>

namespace numeric
{
    constexpr unsigned long const version = 1UL;
    #ifdef DEBUG
    constexpr unsigned long const debug_mode = 1;
    #else
    constexpr unsigned long const debug_mode = 0;
    #endif

    namespace float16_t_private
    {

        union float16
        {
            std::uint16_t bits_;
            struct
            {
                std::uint16_t frac_ : 10;
                std::uint16_t exp_  : 5;
                std::uint16_t sign_ : 1;
            } ieee_;
        };

        template< std::integral T >
        constexpr float16 to_float16( T val ) noexcept
        {
            return float16{ static_cast<std::uint16_t>( val ) };
        }

        inline std::ostream& operator<<( std::ostream& os, float16 const& f16 )
        {
            os << std::bitset<1>( f16.ieee_.sign_ ) << " "
               << std::bitset<5>( f16.ieee_.exp_ ) << " "
               << std::bitset<10>( f16.ieee_.frac_ );
            return os;
        }

        union float32
        {
            std::uint32_t bits_;
            struct
            {
                uint32_t frac_ : 23;
                uint32_t exp_  : 8;
                uint32_t sign_ : 1;
            } ieee_;
            float float_;
        };

        inline std::ostream& operator<<( std::ostream& os, float32 const& f32 )
        {
            os << std::bitset<1>( f32.ieee_.sign_ ) << " "
               << std::bitset<8>( f32.ieee_.exp_ ) << " "
               << std::bitset<23>( f32.ieee_.frac_ ) << " ("
               << f32.float_ << ")";
            return os;
        }

        inline constexpr float16 float32_to_float16( float input ) noexcept
        {
            using float16_t_private::to_float16;
            float32 f32;
            f32.float_ = input;
            std::uint32_t const u32 = f32.bits_;
            std::uint32_t sign = u32 & 0x80000000;
            std::uint32_t exponent = u32 & 0x7f800000;
            std::uint32_t coef = u32 & 0x7fffff;

            if ( exponent == 0x7f800000 )
            {
                std::uint32_t nan_bit = 0;

                if ( coef != 0 )
                    nan_bit = 0x200;

                return to_float16( ( sign >> 16 ) | 0x7c00 | nan_bit | ( coef >> 13 ) );
            }

            std::uint32_t half_sign = sign >> 16;
            std::int32_t unbiased_exponent = static_cast<std::uint32_t>( exponent >> 23 ) - 127;
            std::int32_t half_exponent = unbiased_exponent + 15;

            if ( half_exponent == 0x1f )
                return to_float16( half_sign | 0x7c00 );

            if ( half_exponent <= 0 )
            {
                if ( half_exponent < -10 )
                    return to_float16( half_sign  );

                std::uint32_t c = coef | 0x800000;
                std::uint32_t half_coef = c >> ( 14 - half_exponent );
                std::uint32_t round_bit = 1 << ( 13 - half_exponent );

                if ( ( c & round_bit ) && ( c & ( 3 * round_bit - 1 ) ) )
                    ++half_coef;

                return to_float16( half_sign | half_coef );
            }

            std::uint32_t uhalf_exponent = half_exponent << 10;
            std::uint32_t half_coef = coef >> 13;
            std::uint32_t round_bit = 0x1000;

            if ( ( coef & round_bit ) && ( coef & ( 3 * round_bit - 1 ) ) )
                return to_float16( ( half_sign | uhalf_exponent | half_coef ) + 1 );

            return to_float16( ( half_sign | uhalf_exponent | half_coef ) );
        }

        inline constexpr float32 float16_to_float32( std::uint16_t input ) noexcept
        {
            std::uint32_t sign = ( input & 0x8000 ) << 16;
            std::uint32_t exponent =  ( input & 0x7c00 ) >> 10;
            std::uint32_t coef = ( input & 0x3ff ) << 13;

            if ( exponent == 0x1f )
            {
                if ( coef == 0 )
                    return float32{ sign | 0x7f800000 | coef };

                return float32{ sign | 0x7fc00000 | coef };
            }

            if ( exponent == 0 )
            {
                if ( coef == 0 )
                    return float32{ sign };

                ++exponent;

                while( coef & 0x7f800000 )
                {
                    coef <<= 1;
                    --exponent;
                }

                coef &= 0x7fffff;
            }

            return float32{ sign | ( exponent + 0x70 ) << 23 | coef };
        }

    }//namespace float16_private

    struct float16_t
    {
        float16_t_private::float16 data_;

        constexpr float16_t() noexcept = default;
        constexpr float16_t( float16_t const& ) noexcept = default;
        constexpr float16_t( float16_t&& ) noexcept = default;
        constexpr float16_t( float other ) noexcept : data_ { float16_t_private::float32_to_float16( other ) } { }
        constexpr float16_t( std::uint16_t bits ) noexcept : data_{ bits } { }

        constexpr float16_t& operator = ( float16_t const& ) noexcept = default;
        constexpr float16_t& operator = ( float16_t&& ) noexcept = default;
        constexpr float16_t& operator = ( std::uint16_t bits ) noexcept
        {
            data_.bits_ = bits;
            return *this;
        }
        constexpr float16_t& operator = ( float other ) noexcept
        {
            data_ = float16_t_private::float32_to_float16( other );
            return *this;
        }

        constexpr operator float() const noexcept
        {
            auto f32 = float16_t_private::float16_to_float32( data_.bits_ );
            return f32.float_;
        }

        constexpr operator std::uint16_t() const noexcept
        {
            return data_.bits_;
        }

        constexpr float16_t& operator += ( float16_t v ) noexcept
        {
            *this = float(*this) + float(v);
            return *this;
        }

        constexpr float16_t& operator -= ( float16_t v ) noexcept
        {
            *this = float(*this) - float(v);
            return *this;
        }

        constexpr float16_t& operator *= ( float16_t v ) noexcept
        {
            *this = float(*this) * float(v);
            return *this;
        }

        constexpr float16_t& operator /= ( float16_t v ) noexcept
        {
            *this = float(*this) / float(v);
            return *this;
        }

        constexpr float16_t& operator += ( float v ) noexcept
        {
            *this = float(*this) + v;
            return *this;
        }

        constexpr float16_t& operator -= ( float v ) noexcept
        {
            *this = float(*this) - v;
            return *this;
        }

        constexpr float16_t& operator *= ( float v ) noexcept
        {
            *this = float(*this) * v;
            return *this;
        }

        constexpr float16_t& operator /= ( float v ) noexcept
        {
            *this = float(*this) / v;
            return *this;
        }

        constexpr float16_t operator -- () noexcept //--f
        {
            *this -= 1.0f;
            return *this;
        }

        constexpr float16_t operator -- (int) noexcept // f--
        {
            float16_t ans{*this};
            *this -= 1.0f;
            return ans;
        }

        constexpr float16_t operator ++ () noexcept //++f
        {
            *this += 1.0f;
            return *this;
        }

        constexpr float16_t operator ++ (int) noexcept // f++
        {
            float16_t ans{*this};
            *this += 1.0f;
            return ans;
        }

        constexpr float16_t operator - () const noexcept
        {
            return float16_t{ static_cast<std::uint16_t>((data_.bits_ & 0x7fff) | (data_.bits_ ^ 0x8000 )) };
        }

        constexpr float16_t operator + () const noexcept
        {
            return *this;
        }

    }; //struct float16_t

    constexpr float16_t         fp16_infinity{ static_cast<std::uint16_t>(0x7c00) };
    constexpr float16_t         fp16_max{ static_cast<std::uint16_t>(0x7bff) }; //65504
    constexpr float16_t         fp16_max_subnormal{ static_cast<std::uint16_t>(0x3ff) }; // 0.00006097
    constexpr float16_t         fp16_min{ static_cast<std::uint16_t>(0xfbff) };
    constexpr float16_t         fp16_min_positive{ static_cast<std::uint16_t>(0x400) };
    constexpr float16_t         fp16_min_positive_subnormal{ static_cast<std::uint16_t>(0x1) };
    constexpr float16_t         fp16_nan{ static_cast<std::uint16_t>(0x7e00) };
    constexpr float16_t         fp16_infinity_negative{ static_cast<std::uint16_t>(0xfc00) };

    constexpr float16_t         fp16_one{ static_cast<std::uint16_t>(0x3c00) };
    constexpr float16_t         fp16_zero{ static_cast<std::uint16_t>(0x0) };
    constexpr float16_t         fp16_zero_negative{ static_cast<std::uint16_t>(0x8000) };
    constexpr float16_t         fp16_e{ static_cast<std::uint16_t>(0x4170) };
    constexpr float16_t         fp16_pi{ static_cast<std::uint16_t>(0x4248) };

    constexpr bool is_nan( float16_t f16 ) noexcept
    {
        return (std::uint16_t(f16) & 0x7fff) > 0x7f80;
    }

    constexpr bool is_inf( float16_t f16 ) noexcept
    {
        return (std::uint16_t(f16) & 0x7fff) == 0x7f80;
    }

    constexpr bool is_isfinite( float16_t f16 ) noexcept
    {
        return (std::uint16_t(f16) & 0x7f80) != 0x7f80;
    }

    constexpr bool is_normal( float16_t f16 ) noexcept
    {
        auto const exponent = std::uint16_t(f16) & 0x7f80;
        return (exponent != 0x7f80) && (exponent != 0);
    }

    constexpr bool is_positive( float16_t f16 ) noexcept
    {
        return ((std::uint16_t(f16)) & 0x8000) == 0;
    }

    constexpr bool is_negative( float16_t f16 ) noexcept
    {
        return (std::uint16_t(f16)) & 0x8000;
    }

    constexpr float16_t operator + ( float16_t lhs, float16_t rhs ) noexcept
    {
        return float(lhs) + float(rhs);
    }

    constexpr float16_t operator - ( float16_t lhs, float16_t rhs ) noexcept
    {
        return float(lhs) - float(rhs);
    }

    constexpr float16_t operator * ( float16_t lhs, float16_t rhs ) noexcept
    {
        return float(lhs) * float(rhs);
    }

    constexpr float16_t operator / ( float16_t lhs, float16_t rhs ) noexcept
    {
        return float(lhs) / float(rhs);
    }

    constexpr bool operator < ( float16_t lhs, float16_t rhs ) noexcept
    {
        return float(lhs) < float(rhs);
    }

    constexpr bool operator <= ( float16_t lhs, float16_t rhs ) noexcept
    {
        return float(lhs) <= float(rhs);
    }

    constexpr bool operator == ( float16_t lhs, float16_t rhs ) noexcept
    {
        return float(lhs) == float(rhs);
    }

    constexpr bool operator > ( float16_t lhs, float16_t rhs ) noexcept
    {
        return float(lhs) > float(rhs);
    }

    constexpr bool operator >= ( float16_t lhs, float16_t rhs ) noexcept
    {
        return float(lhs) >= float(rhs);
    }

    template<typename CharT, class Traits>
    std::basic_ostream<CharT, Traits>& operator << ( std::basic_ostream<CharT, Traits>& os, float16_t const& f )
    {
        std::basic_ostringstream<CharT, Traits> __s;
        __s.flags(os.flags());
        __s.imbue(os.getloc());
        __s.precision(os.precision());

        __s << float(f);
        if constexpr( debug_mode )
        {
            __s << "(";
            __s  << std::bitset<1>( f.data_.ieee_.sign_ ) << " ";
            __s  << std::bitset<5>( f.data_.ieee_.exp_ ) << " ";
            __s  << std::bitset<10>( f.data_.ieee_.frac_ ) << ")";
        }

        return os << __s.str();
    }

    template<typename CharT, class Traits>
    std::basic_istream<CharT, Traits>& operator >> ( std::basic_istream<CharT, Traits>& is, float16_t& f )
    {
        bool fail = true;
        float v;

        if ( is >> v )
        {
            fail = false;
            f = v;
        }

        if (fail)
            is.setstate(std::ios_base::failbit);

        return is;
    }


    //TODO: all functions in <cmath>

    constexpr float16_t abs( float16_t f ) noexcept
    {
        float16_t ans{f};
        ans.data_.bits_ &= 0x7fff;
        return ans;
    }



}//namespace numeric


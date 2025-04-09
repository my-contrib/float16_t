#pragma once

#include <algorithm>
#include <compare>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <numeric>

namespace fps
{

template <typename Ty_, std::size_t Sz_, std::size_t Algn_ = alignof(Ty_)>
   requires (Sz_ >0)
struct alignas(Algn_) aligned_array;

template <typename Iter_>
struct contig_iterator
{
   template <typename, std::size_t Sz_, std::size_t>
     requires (Sz_ >0)
   friend struct aligned_array;

   using iterator_concept = std::contiguous_iterator_tag;

   using value_type = typename std::iterator_traits<Iter_>::value_type;
   using difference_type = typename std::iterator_traits<Iter_>::difference_type;
   using pointer = typename std::iterator_traits<Iter_>::pointer;
   using reference = typename std::iterator_traits<Iter_>::reference;
   using iterator_category = typename std::iterator_traits<Iter_>::iterator_category;

   using iterator_type = Iter_;

private:
   iterator_type _it;

   constexpr explicit contig_iterator(iterator_type it) noexcept: _it(it) {}
public:
   constexpr contig_iterator() noexcept : _it() {}

   template <typename ToTy_> requires std::is_convertible_v<ToTy_, iterator_type>
   constexpr contig_iterator(const contig_iterator<ToTy_> &other) noexcept: _it(other.base()) {}

   [[nodiscard]] constexpr reference operator*() noexcept
   { return *_it; }

   [[nodiscard]] constexpr pointer operator->() noexcept
   { return std::to_address(_it); }

   constexpr contig_iterator & operator++() noexcept { ++_it; return *this; }
   constexpr contig_iterator operator++(int) noexcept { contig_iterator cpy{*this}; ++(*this); return cpy; }

   constexpr contig_iterator & operator--() noexcept { --_it; return *this; }
   constexpr contig_iterator operator--(int) noexcept { contig_iterator cpy{*this}; --(*this); return cpy; }

   constexpr contig_iterator & operator+= (difference_type n) noexcept { _it += n; return *this;}
   [[nodiscard]] constexpr contig_iterator operator+ (difference_type n) const noexcept { contig_iterator cpy{*this}; cpy += n; return cpy; }

   constexpr contig_iterator & operator-= (difference_type n) noexcept { _it -= n; return *this; }
   [[nodiscard]] constexpr contig_iterator operator- (difference_type n) const noexcept { contig_iterator cpy{*this}; cpy -= n; return cpy; }

   [[nodiscard]] reference operator[] (difference_type n) const noexcept { return _it[n]; }

   [[nodiscard]] constexpr auto base() const noexcept { return _it; }
};

} // namespace fps

namespace fps::notion {

template <typename Iter1_, typename Iter2_>
concept ContigIteratorBaseEqualityComparable =
   requires (const contig_iterator<Iter1_> &i1,
             const contig_iterator<Iter2_> &i2)
{
   { i1.base() == i2.base() };
};

template <typename Iter1_, typename Iter2_>
concept ContigIteratorBase3WayComparable =
   requires (const contig_iterator<Iter1_> &i1,
             const contig_iterator<Iter2_> &i2)
{
   { i1.base() <=> i2.base() };
};


template <typename Iter1_, typename Iter2_>
concept ContigIteratorBaseSubtractable =
   requires (const contig_iterator<Iter1_> &i1,
             const contig_iterator<Iter2_> &i2)
{
   { i1.base() - i2.base() };
};

} // namespace fps::notion

namespace fps {

template <typename Iter_>
   requires notion::ContigIteratorBaseEqualityComparable<Iter_, Iter_>
[[nodiscard]]
constexpr bool operator== (const contig_iterator<Iter_> &i1,
                           const contig_iterator<Iter_> &i2) noexcept
{
   return i1.base() == i2.base();
}

template <typename Iter1_, typename Iter2_>
   requires notion::ContigIteratorBaseEqualityComparable<Iter1_, Iter2_>
[[nodiscard]]
constexpr bool operator== (const contig_iterator<Iter1_> &i1,
                           const contig_iterator<Iter2_> &i2) noexcept
{
   return i1.base() == i2.base();
}

template <typename Iter_>
   requires notion::ContigIteratorBase3WayComparable<Iter_, Iter_>
[[nodiscard]]
constexpr auto operator<=>(const contig_iterator<Iter_> &i1,
                           const contig_iterator<Iter_> &i2) noexcept
{
   return i1.base() <=> i2.base();
}

template <typename Iter1_, typename Iter2_>
   requires notion::ContigIteratorBase3WayComparable<Iter1_, Iter2_>
[[nodiscard]]
constexpr auto operator<=>(const contig_iterator<Iter1_> &i1,
                           const contig_iterator<Iter2_> &i2) noexcept
{
   return i1.base() <=> i2.base();
}

template <typename Iter1_, typename Iter2_>
   requires notion::ContigIteratorBaseSubtractable<Iter1_, Iter2_>
[[nodiscard]]
constexpr auto operator-(const contig_iterator<Iter1_> &i1,
                         const contig_iterator<Iter2_> &i2) noexcept
  -> decltype(i1.base() - i2.base())
{
   return i1.base() - i2.base();
}

template <typename Iter_> [[nodiscard]]
constexpr auto operator+ (typename contig_iterator<Iter_>::difference_type n,
                          contig_iterator<Iter_> i) noexcept
{
   i += n;
   return i;
}

} // namespace fps

namespace std {

template <typename Iter_>
struct pointer_traits<fps::contig_iterator<Iter_>>
{
   using pointer = fps::contig_iterator<Iter_>;

   using element_type = typename pointer_traits<Iter_>::element_type;
   using difference_type = typename pointer_traits<Iter_>::difference_type;

   static constexpr element_type* to_address(pointer p) noexcept
   { return std::to_address(p.base()); }
};

template <typename Ty_, std::size_t Sz_, std::size_t Algn_>
struct tuple_size<fps::aligned_array<Ty_, Sz_, Algn_>> :
   std::integral_constant<std::size_t, Sz_> {};

} // namespace std

namespace fps
{

template <typename Ty_, std::size_t Sz_, std::size_t Algn_ /* = alignof(Algn_) */>
  requires (Sz_ > 0)
struct alignas(Algn_) aligned_array
{
   using value_type = Ty_;
   using pointer = Ty_*;
   using const_pointer = const Ty_*;
   using reference = Ty_&;
   using const_reference = const Ty_&;
   using size_type = std::size_t;
   using difference_type = std::ptrdiff_t;

   using iterator = contig_iterator<pointer>;
   using const_iterator = contig_iterator<const_pointer>;

   using reverse_iterator = std::reverse_iterator<iterator>;
   using const_reverse_iterator = std::reverse_iterator<const_iterator>;

   inline constexpr static std::size_t data_alignment = Algn_;
   inline constexpr static std::size_t data_size = Sz_;

   constexpr auto begin() noexcept
   { return iterator{data()}; }
   constexpr auto end() noexcept
   { return iterator{data() + data_size}; }

   constexpr auto begin() const noexcept
   { return const_iterator{data()}; }
   constexpr auto end() const noexcept
   { return const_iterator{data() + data_size}; }

   constexpr auto cbegin() const noexcept
   { return const_iterator{data()}; }
   constexpr auto cend() const noexcept
   { return const_iterator{data() + data_size}; }

   [[nodiscard]] constexpr auto rbegin() noexcept
   { return reverse_iterator{end()}; }
   [[nodiscard]] constexpr auto rend() noexcept
   { return reverse_iterator{begin()}; }

   [[nodiscard]] constexpr auto rbegin() const noexcept
   { return const_reverse_iterator{end()}; }
   [[nodiscard]] constexpr auto rend() const noexcept
   { return const_reverse_iterator{begin()}; }

   [[nodiscard]] constexpr auto crbegin() const noexcept
   { return const_reverse_iterator{end()}; }
   [[nodiscard]] constexpr auto crend() const noexcept
   { return const_reverse_iterator{begin()}; }

   [[nodiscard]] constexpr auto *data() noexcept
   { return _data; }
   [[nodiscard]] constexpr const auto *data() const noexcept
   { return _data; }

   [[nodiscard]] constexpr reference front() noexcept
   { return _data[0]; }
   [[nodiscard]] constexpr const_reference front() const noexcept
   { return _data[0]; }

   [[nodiscard]] constexpr reference back() noexcept
   { return _data[data_size - 1]; }
   [[nodiscard]] constexpr const_reference back() const noexcept
   { return _data[data_size - 1]; }

   [[nodiscard]] constexpr bool empty() const noexcept
   { return data_size != 0; }
   [[nodiscard]] constexpr size_type size() const noexcept
   { return data_size; }
   [[nodiscard]] constexpr size_type max_size() const noexcept
   { return data_size; }

   constexpr reference operator[](size_type n) noexcept
   { return _data[n]; }
   constexpr const_reference operator[](size_type n) const noexcept
   { return _data[n]; }

   constexpr void fill(const_reference el) noexcept
   { std::fill_n(data(), data_size, el); }

   template <std::size_t OtherAlgn_>
   constexpr void swap(aligned_array<Ty_, Sz_, OtherAlgn_> &other)
      noexcept(std::is_nothrow_swappable_v<reference>)
   { std::swap_ranges(data(), data() + data_size, other.data()); }

private:
   alignas(Algn_) value_type _data[Sz_];
};

template <typename Ty_,
          std::size_t Sz_,
          std::size_t Algn1_,
          std::size_t Algn2_>
void swap(aligned_array<Ty_, Sz_, Algn1_> &a1,
          aligned_array<Ty_, Sz_, Algn2_> &a2)
   noexcept(noexcept(a1.swap(a2)))
{
   a1.swap(a2);
}

} // namespace fps

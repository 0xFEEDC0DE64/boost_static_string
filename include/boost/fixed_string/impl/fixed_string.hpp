//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/fixed_string
//

#ifndef BOOST_FIXED_STRING_IMPL_FIXED_STRING_HPP
#define BOOST_FIXED_STRING_IMPL_FIXED_STRING_HPP

#include <boost/fixed_string/detail/fixed_string.hpp>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>
#include <boost/throw_exception.hpp>

namespace boost {
namespace fixed_string {

//------------------------------------------------------------------------------
//
// Construction
//
//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
fixed_string<N, CharT, Traits>::
fixed_string()
{
    n_ = 0;
    term();
}

template<std::size_t N, typename CharT, typename Traits>
fixed_string<N, CharT, Traits>::
fixed_string(size_type count, CharT ch)
{
    assign(count, ch);
}

template<std::size_t N, typename CharT, typename Traits>
template<std::size_t M>
fixed_string<N, CharT, Traits>::
fixed_string(fixed_string<M, CharT, Traits> const& other,
    size_type pos)
{
    assign(other, pos);
}

template<std::size_t N, typename CharT, typename Traits>
template<std::size_t M>
fixed_string<N, CharT, Traits>::
fixed_string(
    fixed_string<M, CharT, Traits> const& other,
    size_type pos,
    size_type count)
{
    assign(other, pos, count);
}

template<std::size_t N, typename CharT, typename Traits>
fixed_string<N, CharT, Traits>::
fixed_string(CharT const* s, size_type count)
{
    assign(s, count);
}

template<std::size_t N, typename CharT, typename Traits>
fixed_string<N, CharT, Traits>::
fixed_string(CharT const* s)
{
    auto const count = Traits::length(s);
    if(count > max_size())
        BOOST_THROW_EXCEPTION(std::length_error{
            "count > max_size()"});
    n_ = count;
    Traits::copy(&s_[0], s, n_ + 1);
}

template<std::size_t N, typename CharT, typename Traits>
template<class InputIt>
fixed_string<N, CharT, Traits>::
fixed_string(
    InputIt first,
    InputIt last,
    typename std::enable_if<
        detail::is_input_iterator<InputIt>::value,
            iterator>::type*)
{
    assign(first, last);
}

template<std::size_t N, typename CharT, typename Traits>
fixed_string<N, CharT, Traits>::
fixed_string(
    fixed_string const& s)
{
    assign(s);
}

template<std::size_t N, typename CharT, typename Traits>
template<std::size_t M>
fixed_string<N, CharT, Traits>::
fixed_string(
    fixed_string<M, CharT, Traits> const& s)
{
    assign(s);
}

template<std::size_t N, typename CharT, typename Traits>
fixed_string<N, CharT, Traits>::
fixed_string(std::initializer_list<CharT> init)
{
    assign(init.begin(), init.end());
}

template<std::size_t N, typename CharT, typename Traits>
fixed_string<N, CharT, Traits>::
fixed_string(string_view_type sv)
{
    assign(sv);
}

template<std::size_t N, typename CharT, typename Traits>
template<class T, class>
fixed_string<N, CharT, Traits>::
fixed_string(T const& t, size_type pos, size_type n)
{
    assign(t, pos, n);
}

//------------------------------------------------------------------------------
//
// Assignment
//
//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
assign(
    size_type count,
    CharT ch) ->
        fixed_string&
{
    if(count > max_size())
        BOOST_THROW_EXCEPTION(std::length_error{
            "count > max_size()"});
    n_ = count;
    Traits::assign(&s_[0], n_, ch);
    term();
    return *this;
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
assign(
    fixed_string const& s) noexcept ->
        fixed_string&
{
    if(this == &s)
        return *this;
    n_ = s.n_;
    auto const n = n_ + 1;
    // VFALCO This informs the static analyzer
    //BOOST_BEAST_ASSUME(n != 0);
    Traits::copy(&s_[0], &s.s_[0], n);
    return *this;
}

template<std::size_t N, typename CharT, typename Traits>
template<std::size_t M>
auto
fixed_string<N, CharT, Traits>::
assign(
    fixed_string<M, CharT, Traits> const& s,
    size_type pos,
    size_type count) ->
    fixed_string&
{
    auto const ss = s.substr(pos, count);
    return assign(ss.data(), ss.size());
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
assign(
    CharT const* s,
    size_type count) ->
        fixed_string&
{
    if(count > max_size())
        BOOST_THROW_EXCEPTION(std::length_error{
            "count > max_size()"});
    n_ = count;
    Traits::copy(&s_[0], s, n_);
    term();
    return *this;
}

template<std::size_t N, typename CharT, typename Traits>
template<class InputIt>
auto
fixed_string<N, CharT, Traits>::
assign(
    InputIt first,
    InputIt last) ->
        typename std::enable_if<
            detail::is_input_iterator<InputIt>::value,
                fixed_string&>::type
{
    std::size_t const n = std::distance(first, last);
    if(n > max_size())
        BOOST_THROW_EXCEPTION(std::length_error{
            "n > max_size()"});
    n_ = n;
    for(auto it = &s_[0]; first != last; ++it, ++first)
        Traits::assign(*it, *first);
    term();
    return *this;
}

//------------------------------------------------------------------------------
//
// Element access
//
//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
at(size_type pos) ->
    reference
{
    if(pos >= size())
        BOOST_THROW_EXCEPTION(std::out_of_range{
            "pos >= size()"});
    return s_[pos];
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
at(size_type pos) const ->
    const_reference
{
    if(pos >= size())
        BOOST_THROW_EXCEPTION(std::out_of_range{
            "pos >= size()"});
    return s_[pos];
}

//------------------------------------------------------------------------------
//
// Capacity
//
//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
void
fixed_string<N, CharT, Traits>::
reserve(std::size_t n)
{
    if(n > max_size())
        BOOST_THROW_EXCEPTION(std::length_error{
            "n > max_size()"});
}

//------------------------------------------------------------------------------
//
// Operations
//
//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
void
fixed_string<N, CharT, Traits>::
clear()
{
    n_ = 0;
    term();
}

//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
insert(
    size_type index,
    size_type count,
    CharT ch) ->
        fixed_string&
{
    if(index > size())
        BOOST_THROW_EXCEPTION(std::out_of_range{
            "index > size()"});
    insert(begin() + index, count, ch);
    return *this;
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
insert(
    size_type index,
    CharT const* s,
    size_type count) ->
        fixed_string&
{
    if(index > size())
        BOOST_THROW_EXCEPTION(std::out_of_range{
            "index > size()"});
    if(size() + count > max_size())
        BOOST_THROW_EXCEPTION(std::length_error{
            "size() + count > max_size()"});
    Traits::move(
        &s_[index + count], &s_[index], size() - index);
    n_ += count;
    Traits::copy(&s_[index], s, count);
    term();
    return *this;
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
insert(
    const_iterator pos,
    size_type count,
    CharT ch) ->
        iterator
{
    if(size() + count > max_size())
        BOOST_THROW_EXCEPTION(std::length_error{
            "size() + count() > max_size()"});
    auto const index = pos - &s_[0];
    Traits::move(
        &s_[index + count], &s_[index], size() - index);
    n_ += count;
    Traits::assign(&s_[index], count, ch);
    term();
    return &s_[index];
}

template<std::size_t N, typename CharT, typename Traits>
template<class InputIt>
auto
fixed_string<N, CharT, Traits>::
insert(
    const_iterator pos,
    InputIt first,
    InputIt last) ->
        typename std::enable_if<
            detail::is_input_iterator<
                InputIt>::value, iterator>::type
{
    std::size_t const count = std::distance(first, last);
    if(size() + count > max_size())
        BOOST_THROW_EXCEPTION(std::length_error{
            "size() + count > max_size()"});
    std::size_t const index = pos - begin();
    Traits::move(
        &s_[index + count], &s_[index], size() - index);
    n_ += count;
    for(auto it = begin() + index;
            first != last; ++it, ++first)
        Traits::assign(*it, *first);
    term();
    return begin() + index;
}

template<std::size_t N, typename CharT, typename Traits>
template<class T>
auto
fixed_string<N, CharT, Traits>::
insert(
    size_type index,
    T const & t) ->
        typename std::enable_if<
            std::is_convertible<
                T const&, string_view_type>::value &&
            ! std::is_convertible<
                T const&, CharT const*>::value, fixed_string&
                    >::type
{
    return insert(index, t, 0, npos);
}

template<std::size_t N, typename CharT, typename Traits>
template<class T>
auto
fixed_string<N, CharT, Traits>::
insert(
    size_type index,
    T const & t,
    size_type index_str,
    size_type count) ->
        typename std::enable_if<
            std::is_convertible<
                T const&, string_view_type>::value &&
            ! std::is_convertible<
                T const&, CharT const*>::value, fixed_string&
                    >::type
{
    auto const s =
        string_view_type(t).substr(index_str, count);
    return insert(index, s.data(), s.size());
}

//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
erase(
    size_type index,
    size_type count) ->
        fixed_string&
{
    if(index > size())
        BOOST_THROW_EXCEPTION(std::out_of_range{
            "index > size()"});
    auto const n = (std::min)(count, size() - index);
    Traits::move(
        &s_[index], &s_[index + n], size() - (index + n) + 1);
    n_ -= n;
    return *this;
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
erase(
    const_iterator pos) ->
        iterator
{
    erase(pos - begin(), 1);
    return begin() + (pos - begin());
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
erase(
    const_iterator first,
    const_iterator last) ->
        iterator
{
    erase(first - begin(),
        std::distance(first, last));
    return begin() + (first - begin());
}

template<std::size_t N, typename CharT, typename Traits>
void
fixed_string<N, CharT, Traits>::
push_back(
    CharT ch)
{
    if(size() >= max_size())
        BOOST_THROW_EXCEPTION(std::length_error{
            "size() >= max_size()"});
    Traits::assign(s_[n_++], ch);
    term();
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
append(
    CharT const* s,
    size_type count) ->
        fixed_string&
{
    if(size() + count > max_size())
        BOOST_THROW_EXCEPTION(std::length_error{
            "size() + count > max_size()"});
    Traits::move(
        &s_[n_ + count], &s_[n_], size() - n_);
    Traits::copy(&s_[n_], s, count);
    n_ += count;
    term();
    return *this;
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
substr(size_type pos, size_type count) const ->
    string_view_type
{
    if(pos > size())
        BOOST_THROW_EXCEPTION(std::out_of_range{
            "pos > size()"});
    return{&s_[pos], (std::min)(count, size() - pos)};
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
copy(CharT* dest, size_type count, size_type pos) const ->
    size_type
{
    auto const s = substr(pos, count);
    Traits::copy(dest, s.data(), s.size());
    return s.size();
}

template<std::size_t N, typename CharT, typename Traits>
void
fixed_string<N, CharT, Traits>::
resize(std::size_t n)
{
    if(n > max_size())
        BOOST_THROW_EXCEPTION(std::length_error{
            "n > max_size()"});
    if(n > n_)
        Traits::assign(&s_[n_], n - n_, CharT{});
    n_ = n;
    term();
}

template<std::size_t N, typename CharT, typename Traits>
void
fixed_string<N, CharT, Traits>::
resize(std::size_t n, CharT c)
{
    if(n > max_size())
        BOOST_THROW_EXCEPTION(std::length_error{
            "n > max_size()"});
    if(n > n_)
        Traits::assign(&s_[n_], n - n_, c);
    n_ = n;
    term();
}

template<std::size_t N, typename CharT, typename Traits>
void
fixed_string<N, CharT, Traits>::
swap(fixed_string& s)
{
    fixed_string tmp(s);
    s.n_ = n_;
    Traits::copy(&s.s_[0], &s_[0], n_ + 1);
    n_ = tmp.n_;
    Traits::copy(&s_[0], &tmp.s_[0], n_ + 1);
}

template<std::size_t N, typename CharT, typename Traits>
template<std::size_t M>
void
fixed_string<N, CharT, Traits>::
swap(fixed_string<M, CharT, Traits>& s)
{
    if(size() > s.max_size())
        BOOST_THROW_EXCEPTION(std::length_error{
            "size() > s.max_size()"});
    if(s.size() > max_size())
        BOOST_THROW_EXCEPTION(std::length_error{
            "s.size() > max_size()"});
    fixed_string tmp(s);
    s.n_ = n_;
    Traits::copy(&s.s_[0], &s_[0], n_ + 1);
    n_ = tmp.n_;
    Traits::copy(&s_[0], &tmp.s_[0], n_ + 1);
}


template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
assign_char(CharT ch, std::true_type) ->
    fixed_string&
{
    n_ = 1;
    Traits::assign(s_[0], ch);
    term();
    return *this;
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
assign_char(CharT, std::false_type) ->
    fixed_string&
{
    BOOST_THROW_EXCEPTION(std::length_error{
        "max_size() == 0"});
}

template<class Integer, class>
fixed_string<detail::max_digits(sizeof(Integer))>
to_fixed_string(Integer x)
{
    using CharT = char;
    using Traits = std::char_traits<CharT>;
    BOOST_STATIC_ASSERT(std::is_integral<Integer>::value);
    char buf[detail::max_digits(sizeof(Integer))];
    auto last = buf + sizeof(buf);
    auto it = detail::raw_to_string<
        CharT, Integer, Traits>(last, sizeof(buf), x);
    fixed_string<detail::max_digits(sizeof(Integer))> s;
    s.resize(static_cast<std::size_t>(last - it));
    auto p = s.data();
    while(it < last)
        Traits::assign(*p++, *it++);
    return s;
}

} // fixed_string
} // boost

#endif
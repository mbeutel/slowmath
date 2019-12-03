
#ifndef INCLUDED_SLOWMATH_DETAIL_CAST_HPP_
#define INCLUDED_SLOWMATH_DETAIL_CAST_HPP_


#include <system_error> // for errc

#include <slowmath/detail/type_traits.hpp> // for have_same_signedness<>, integral_value_type<>


#if defined(_MSC_VER) && !defined(__clang__)
# pragma warning( push )
//# pragma warning( disable: 4127 ) // conditional expression is constant
# pragma warning( disable: 4702 ) // unreachable code
#endif // defined(_MSC_VER) && !defined(__clang__)


namespace slowmath
{

namespace detail
{


template <typename EH, typename DstT, typename SrcT>
constexpr result_t<EH, DstT> integral_cast(SrcT src)
{
    using SrcV = integral_value_type<SrcT>;

    auto dst = static_cast<DstT>(src);
    if (static_cast<SrcV>(dst) != src
        || (!have_same_signedness_v<DstT, SrcV> && ((dst < 0) != (src < 0))))
    {
        return EH::make_error(std::errc::value_too_large);
    }
    return EH::make_result(dst);
}


} // namespace detail

} // namespace slowmath


#if defined(_MSC_VER) && !defined(__clang__)
 #pragma warning( pop )
#endif // defined(_MSC_VER) && !defined(__clang__)


#endif // INCLUDED_SLOWMATH_DETAIL_CAST_HPP_

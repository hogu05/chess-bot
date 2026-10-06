#ifndef BIT_UTILS_HPP
#define BIT_UTILS_HPP

namespace bit_utils
{

constexpr int mask(int shift, int width)
{
    return ((1 << width) - 1) << shift;
}

} // namespace bit_utils

#endif

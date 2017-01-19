/*!
 * \file
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \date   2016-09-02
 * \brief  File with useful parsing functions.
 */

#ifndef YS_TD_LIB_H
#define YS_TD_LIB_H

#include <string>
#include <vector>
#include <sstream>
#include <locale>

/*!
 * Convert hex-array to string.
 * \param b HEX-array.
 * \param s Array size.
 * \return
 */
std::string
hex2str(void const* b, std::size_t s);

/*!
 * Convert hex-array to unsigned int.
 * \param b HEX-array.
 * \param s Array size.
 * \return
 */
unsigned
hex2uint(void const* b, std::size_t s);

/*!
 * Split string by delimiter.
 * \param s String to split.
 * \param d delimiter.
 * \return Vector of splits.
 */
std::vector<std::string>
split(std::string const& s, char d);

/*!
 * Last `vec2vars` call function.
 * \param v
 */
template<int N>
void
vec2vars(std::vector<std::string> const& v)
{
}

/*!
 * Template function to read values from vector into variables.
 * \param v
 * \param arg
 * \param ...
 */
template<int N = 0, typename Arg, typename ...Args>
void
vec2vars(std::vector<std::string> const& v, Arg& arg, Args&... args)
{
    arg = v[N];
    vec2vars<N+1>(v, args...);
}

/*!
 * Convert string to the specified type using stream operations.
 * \param s Input string.
 * \return Parsed value.
 */
template<typename T>
T
parse_string(std::string const& s)
{
    T v {};
    std::istringstream { s } >> v;
    return v;
}

/*!
 * Get datetime string converted from `in_loc` to `out_loc`.
 * \param dt
 * \param in_loc
 * \param out_loc
 */
std::string
conv_datetime(std::string const& dt,
        std::locale const& in_loc, std::locale const& out_loc);

#endif // YS_TD_LIB_H


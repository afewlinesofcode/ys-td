/*!
 * \file
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \date   2016-09-02
 * \brief  File with useful parsing functions.
 */

#include <ys/td/lib.h>

#include <sstream>
#include <iomanip>
#include <boost/algorithm/string.hpp>
#include <boost/date_time.hpp>

/*!
 * Convert hex sequence to string.
 * \return
 */
std::string
hex2str(void const* b, std::size_t s)
{
    std::ostringstream os;

    os << std::hex;

    char const* c = (char const*)b;

    for (std::size_t i = 0; i < s; ++i)
    {
        os << std::setfill('0') << std::setw(2) << +c[i];
    }

    os << std::dec;

    return os.str();
}

/*!
 * Convert hex-array to unsigned int.
 * \param b HEX-array.
 * \param s Array size.
 * \return
 */
unsigned
hex2uint(void const* b, std::size_t s)
{
    unsigned dec;
    std::string str = hex2str(b, s);
    std::istringstream(str) >> dec;

    return dec;
}

/*!
 * Split string by delimiter.
 * \param s String to split.
 * \param d delimiter.
 * \return Vector of splits.
 */
std::vector<std::string>
split(std::string const& s, char d)
{
    std::vector<std::string> res;
    auto cb = [d](char c)
    {
        return c == d;
    };

    boost::algorithm::split(res, s, cb);

    return res;
}

/*!
 * Get datetime string converted from `in_loc` to `out_loc`.
 * \param dt
 * \param in_loc
 * \param out_loc
 */
std::string
conv_datetime(std::string const& dt,
        std::locale const& in_loc, std::locale const& out_loc)
{
    std::istringstream is { dt };
    std::ostringstream os {};

    is.imbue(in_loc);
    os.imbue(out_loc);

    boost::posix_time::ptime t;

    is >> t;
    os << t;

    return os.str();
}


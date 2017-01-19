/*!
 * \file
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \date   2016-08-04
 * \brief  Glonass trackers daemon error class.
 */

#include <stdexcept>
#include <cstdarg>
#include <cstdio>

#ifndef YS_TD_ERROR_MSG_SIZE
#define YS_TD_ERROR_MSG_SIZE 1024

namespace ys
{
namespace td
{

/*!
 * Error class.
 */
class error: public std::runtime_error
{
public:
    /*!
     * Constructor.
     * \param format Formatted error message.
     */
    error(char const* format, ...)
        : runtime_error("")
    {
        va_list args;

        va_start(args, format);
        std::vsprintf(msg, format, args);
        va_end(args);
    }

    /*!
     * Destructor.
     */
    virtual
    ~error() throw()
    {
    }

    /*!
     * Get error message.
     */
    char const*
    what() const throw()
    {
        return msg;
    }

private:
    char msg[YS_TD_ERROR_MSG_SIZE];
};

} // namespace td
} // namespace ys

#endif // YS_TD_ERROR_MSG_SIZE


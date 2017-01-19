/*!
 * \file
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \date   2016-08-04
 * \brief  Parser base class header file.
 */

#include <ys/td/parser.h>

namespace ys
{
namespace td
{

/*!
 * Construct parser object.
 */
parser::parser()
{
}

/*!
 * Destruct parser object.
 */
parser::~parser()
{
}

/*!
 * Get parsed data.
 * \return
 */
parser::data_type const&
parser::data() const
{
    return data_;
}

/*!
 * Set a type name.
 */
void
parser::type(std::string const& name)
{
    data_.type = name;
}

/*!
 * Get a reference to a parser buffer.
 * \return
 */
parser::buffer_type const&
parser::buffer() const
{
    return buffer_;
}

/*!
 * Get a reference to the response buffer.
 * \return
 */
parser::buffer_type&
parser::response()
{
    return response_;
}

/*!
 * Erase `n` bytes from the beginning of the buffer.
 * \param n
 */
void
parser::consume(std::size_t n)
{
    if (n > buffer_.size())
    {
        buffer_.clear();
    }
    else
    {
        auto it = buffer_.begin();

        buffer_.erase(it, std::next(it, n));
    }
}

/*!
 * Erase bytes from the buffer in the range `[begin(), it)`.
 * \param it
 */
void
parser::consume(buffer_type::iterator it)
{
    buffer_.erase(buffer_.begin(), it);
}

/*!
 * Erase all bytes in the buffer.
 */
void
parser::consume()
{
    buffer_.clear();
}

} // namespace td
} // namespace ys


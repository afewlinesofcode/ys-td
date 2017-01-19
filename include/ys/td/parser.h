/*!
 * \file
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \date   2016-08-04
 * \brief  Parser base class header file.
 */

#ifndef YS_TD_PARSER_H
#define YS_TD_PARSER_H

#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <iterator>
#include <type_traits>

namespace ys
{
namespace td
{

/*!
 * Parser base class.
 */
class parser
{
public:
    /*!
     * Buffer typedef.
     */
    using buffer_type = std::vector<uint8_t>;

    /*!
     * Parsing result structure.
     */
    struct result_type
    {
        /*!
         * Message was parsed.
         */
        bool parsed { true };

        /*!
         * Parsing was ok but there is nothing to put into a database.
         */
        bool skip { false };

        /*!
         * Parsing failed, everything is bad, connection must leave.
         */
        bool corrupt { false };
    };

    /*!
     * Parsed data.
     */
    struct data_type
    {
        /*!
         * Tracker phone number.
         */
        std::string phone;

        /*!
         * Tracker number.
         */
        std::string num;

        /*!
         * Tracker type name.
         */
        std::string type;

        /*!
         * Signal date/time.
         */
        std::string datetime;

        /*!
         * Longitude.
         */
        double lon {};

        /*!
         * Latitude.
         */
        double lat {};

        /*!
         * Speed.
         */
        double speed {};

        /*!
         * Odometer.
         */
        uint32_t odometer {};

        /*!
         * Course.
         */
        double course {};

        /*!
         * Number of GLONASS satellites.
         */
        uint32_t sats_glonass {};

        /*!
         * Number of GPS satellites.
         */
        uint32_t sats_gps {};

        /*!
         * Parsed data output.
         * \param os
         * \param d
         * \return
         */
        friend
        std::ostream& operator<<(std::ostream& os, data_type const& d)
        {
            os <<
                "num " << d.num << ", " <<
                "type " << d.type << ", " <<
                "datetime " << d.datetime << ", " <<
                "lon " << d.lon << ", " <<
                "lat " << d.lat << ", " <<
                "speed " << d.speed << ", " <<
                "odometer " << d.odometer << ", " <<
                "course " << d.course << ", " <<
                "sats_glonass " << d.sats_glonass << ", " <<
                "sats_gps " << d.sats_gps <<
                std::endl;

            return os;
        }
    };

    /*!
     * Construct parser object.
     */
    parser();

    /*!
     * Destruct parser object.
     */
    virtual
    ~parser();

    /*!
     * Load data from the external buffer to the parser buffer.
     * \param b External buffer.
     * \param n Number of bytes to load.
     */
    template<typename Buffer>
    void
    load(Buffer const& b, std::size_t n)
    {
        auto ii = b.begin();
        buffer_.reserve(buffer_.size() + n);
        std::copy(ii, std::next(ii, n), std::back_inserter(buffer_));
    }

    /*!
     * Get parsed data.
     * \return
     */
    data_type const&
    data() const;

    /*!
     * Set a type name.
     */
    void
    type(std::string const& name);

    /*!
     * Get a reference to a parser buffer.
     * \return
     */
    buffer_type const&
    buffer() const;

    /*!
     * Get a reference to the response buffer.
     * \return
     */
    buffer_type&
    response();

    /*!
     * \brief Parse tracker data.
     * \return
     * \throw error
     */
    virtual
    result_type
    parse() = 0;

protected:
    /*!
     * Parsed tracker data.
     */
    data_type data_;

    /*!
     * Buffer with not yet parsed data.
     */
    buffer_type buffer_;

    /*!
     * Vector with response bytes.
     */
    buffer_type response_;

    /*!
     * Erase `n` bytes from the beginning of the buffer.
     * \param n
     */
    void
    consume(std::size_t n);

    /*!
     * Erase bytes from the buffer in the range `[begin(), it)`.
     * \param it
     */
    void
    consume(buffer_type::iterator it);

    /*!
     * Erase all bytes in the buffer.
     */
    void
    consume();

    /*!
     * Add response bytes from POD.
     */
    template<typename POD>
    void
    response(POD const& r,
            typename std::enable_if<std::is_pod<POD>::value>::type* = 0)
    {
        uint8_t *p = (uint8_t*)(&r);
        std::copy(p, p + sizeof(r), std::back_inserter(response_));
    }

    /*!
     * Add response bytes from array.
     */
    template<typename T, int N>
    void
    response(T (&r)[N],
            typename std::enable_if<std::is_arithmetic<T>::value>::type* = 0)
    {
        uint8_t *p = (uint8_t*)r;
        std::copy(p, p + N*sizeof(T), std::back_inserter(response_));
    }

    /*!
     * Add response bytes from string.
     */
    void
    response(std::string const& r)
    {
        std::copy(r.begin(), r.end(), std::back_inserter(response_));
    }
};

} // namespace td
} // namespace ys

#endif // YS_TD_PARSER_H


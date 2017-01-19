/*!
 * \file
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \date   2016-09-04
 * \brief  ST270 parser header file.
 */

#ifndef YS_TD_ST270_PARSER_H
#define YS_TD_ST270_PARSER_H

#include <functional>
#include <locale>

#include <ys/td/parser.h>

namespace ys
{
namespace td
{

class st270_parser: public parser
{
public:
    /*!
     * Constructor.
     */
    st270_parser();

    /*!
     * Parse loaded data.
     */
    parser::result_type
    parse() override;

private:
    /*!
     * Locale for reading datetime strings.
     */
    std::locale in_loc;

    /*!
     * Locale for writing datetime strings.
     */
    std::locale out_loc;

    /*!
     * Get ST270 report string.
    * \return
     */
    std::string
    get_report_string();

    /*!
     * Parse report string.
     * \param s
     * \return
     */
    parser::result_type
    parse_report(std::string const& s);

    /*!
     * Get function for reading packet data.
     * \param hdr Packet header.
     * \return
     */
    std::function<bool(std::vector<std::string>&)>
    get_read_fn(std::string const& hdr);

    /*!
     * Read status report data.
     * \param v Vector with report values.
     * \return
     */
    bool
    read_status_report(std::vector<std::string>& v);

    /*!
     * Read emergency report data.
     * \param v Vector with report values.
     * \return
     */
    bool
    read_emergency_report(std::vector<std::string>& v);

    /*!
     * Read event report data.
     * \param v Vector with report values.
     * \return
     */
    bool
    read_event_report(std::vector<std::string>& v);

    /*!
     * Read alert report data.
     * \param v Vector with report values.
     * \return
     */
    bool
    read_alert_report(std::vector<std::string>& v);

    /*!
     * Read data from the report packet.
     * \param v Vector of report values.
     * \return
     */
    bool
    read_report_data(std::vector<std::string>& v);
};

} // namespace td
} // namespace ys

#endif // YS_TD_ST270_PARSER_H


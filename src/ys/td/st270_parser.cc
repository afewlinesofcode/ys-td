/*!
 * \file
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \date   2016-09-04
 * \brief  ST270 parser source file.
 */

#include <ys/td/st270_parser.h>

#include <algorithm>
#include <iterator>
#include <string>
#include <sstream>

#include <boost/date_time.hpp>

#include <ys/td/lib.h>

namespace ys
{
namespace td
{

/*!
 * Constructor.
 */
st270_parser::st270_parser() :
    in_loc
{
    std::locale::classic(),
    new boost::posix_time::time_input_facet("%Y%m%d %H:%M:%S")
},
out_loc
{
    std::locale::classic(),
    new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%S")
}
{
}

/*!
 * Parse loaded data.
 */
parser::result_type
st270_parser::parse()
{
    std::string report = get_report_string();

    if (report.empty())
        return { false };

    return parse_report(report);
}

/*!
 * Get ST270 report string.
 * \return
 */
std::string
st270_parser::get_report_string()
{
    /*!
     * A pointer to the report delimiter.
     */
    auto it = std::find(buffer_.begin(), buffer_.end(), '\r');

    /*
     * If delimiter was not then then the packet is not full.
     */
    if (it == buffer_.end())
        return "";

    /*!
     * A report string.
     */
    std::string report { buffer_.begin(), it };

    /*
     * Remove the packet data from the buffer.
     */
    consume(std::next(it));

    return report;
}

/*!
 * Get function for reading packet data.
 * \param hdr Packet header.
 * \return
 */
std::function<bool(std::vector<std::string>&)>
st270_parser::get_read_fn(std::string const& hdr)
{
    using namespace std::placeholders;

    if (hdr == "ST270STT")
        return std::bind(&st270_parser::read_status_report, this, _1);

    if (hdr == "ST270EMG")
        return std::bind(&st270_parser::read_emergency_report, this, _1);

    if (hdr == "ST270EVT")
        return std::bind(&st270_parser::read_event_report, this, _1);

    if (hdr == "ST270ALT")
        return std::bind(&st270_parser::read_alert_report, this, _1);

    return nullptr;
}

/*!
 * Parse report string.
 * \param s
 * \return
 */
parser::result_type
st270_parser::parse_report(std::string const& s)
{
    /*!
     * Vector with report values.
     */
    auto v = split(s, ';');

    /*
     * If there are less than two elements then the packet is corrupt.
     */
    if (v.size() < 2)
        return { false, false, true };

    /*!
     * Report header.
     */
    std::string hdr = v[0];

    data_.num = v[1];

    /*
     * Skip parsing in the case of alive report.
     */
    if (hdr == "ST270ALV")
        return { true, true };

    auto fn = get_read_fn(hdr);

    /*
     * If reading function determination failed then the tracker is corrupt.
     */
    if (!fn)
        return { false, false, true };

    return { fn(v) };
}

/*!
 * Read status report data.
 * \param v Vector with report values.
 * \return
 */
bool
st270_parser::read_status_report(std::vector<std::string>& v)
{
    /*
     * Attempt to read tracker data.
     */
    if (!read_report_data(v))
        return false;

    /*
     * Remaining packet variables.
     */
    std::string mode, msg_num;

    vec2vars(v, mode, msg_num);

    return true;
}

/*!
 * Read emergency report data.
 * \param v Vector with report values.
 * \return
 */
bool
st270_parser::read_emergency_report(std::vector<std::string>& v)
{
    /*
     * Attempt to read tracker data.
     */
    if (!read_report_data(v))
        return false;

    /*
     * Remaining packet variables.
     */
    std::string emg_id;

    vec2vars(v, emg_id);

    return true;
}

/*!
 * Read event report data.
 * \param v Vector with report values.
 * \return
 */
bool
st270_parser::read_event_report(std::vector<std::string>& v)
{
    /*
     * Attempt to read tracker data.
     */
    if (!read_report_data(v))
        return false;

    /*
     * Remaining packet variables.
     */
    std::string evt_id;

    vec2vars(v, evt_id);

    return true;
}

/*!
 * Read alert report data.
 * \param v Vector with report values.
 * \return
 */
bool
st270_parser::read_alert_report(std::vector<std::string>& v)
{
    /*
     * Attempt to read tracker data.
     */
    if (!read_report_data(v))
        return false;

    /*
     * Remaining packet variables.
     */
    std::string alert_id;

    vec2vars(v, alert_id);

    return true;
}

/*!
 * Read data from the report packet.
 * \param v Vector of report values.
 * \return
 */
bool
st270_parser::read_report_data(std::vector<std::string>& v)
{
    /*
     * Packet variables.
     */
    std::string sw_ver, date, time, cell, lat, lon, spd, crs,
        satt_gps, satt_glonass, fix, alt, pulse_in1, pulse_in2,
        dist, pwr_volt, io, an1, an2, an3, an4, to, tf, vs;

    /*!
     * Number of read variables.
     */
    const int var_num = 24;

    /*
     * Check whether there is sufficient number of data.
     */
    if (v.size() < var_num)
        return false;

    /*
     * Load data into variables.
     */
    vec2vars(v, sw_ver, date, time, cell, lat, lon, spd, crs,
             satt_gps, satt_glonass, fix, alt, pulse_in1, pulse_in2,
             dist, pwr_volt, io, an1, an2, an3, an4, to, tf, vs);

    /*
     * Fill tracker data variables.
     */
    data_.datetime = conv_datetime(date + " " + time, in_loc, out_loc);
    data_.lat = parse_string<double>(lat);
    data_.lon = parse_string<double>(lon);
    data_.speed = parse_string<double>(spd);
    data_.course = parse_string<double>(crs);
    data_.sats_gps = parse_string<int>(satt_gps);
    data_.sats_glonass = parse_string<int>(satt_glonass);
    data_.odometer = parse_string<int>(dist) / 1000;

    /*
     * Remove processed variables from the vector of report variables.
     */
    v.erase(v.begin(), std::next(v.begin(), var_num));

    return true;
}

} // namespace td
} // namespace ys


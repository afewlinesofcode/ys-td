/*!
 * \file
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \date   2016-08-03
 * \brief  Config class header file.
 */

#ifndef YS_TD_CONFIG_H
#define YS_TD_CONFIG_H

#include <ys/config.h>
#include <vector>
#include <string>
#include <map>

namespace ys
{
namespace td
{

/*!
 * Glonass trackers daemon config class.
 */
class config: public ys::config
{
public:
    /*!
     * Structure describing listening port.
     */
    struct port
    {
        /*!
         * Listening port number.
         */
        int num;

        /*!
         * Name of the parser used on the listening port.
         */
        std::string parser;

        /*!
         * Name of the tracker type.
         */
        std::string type;
    };

    /*!
     * Configuration values.
     */
    struct
    {
        /*!
         * A path to a config-file.
         */
        std::string cfg_path;

        /*!
         * A number of daemon workers.
         */
        int w_count;

        /*!
         * Listen host.
         */
        std::string host;

        /*!
         * Database connection strings.
         */
        std::vector<std::string> db;

        /*!
         * Ports settings.
         */
        std::map<int, port> ports;
    } data;

    /*!
     * Constructor.
     * \param argc
     * \param argv
     */
    config(int argc, char** argv);

    /*!
     * Initialize command-line options.
     */
    void
    init_cmd_options();

    /*!
     * Read config-file options.
     */
    void
    read_cfg_options();

    /*!
     * Load command-line options into variables.
     */
    void
    load_cmd_options();

    /*!
     * Load config-file options into variables.
     */
    void
    load_cfg_options();

private:
    /*!
     * Load ports configuration into variables.
     */
    void
    load_ports_cfg();

    /*!
     * Print out config parameters.
     */
    friend
    std::ostream&
    operator <<(std::ostream& os, config const& c)
    {
        os <<
           "cfg_path: " << c.data.cfg_path << std::endl <<
           "workers: " << c.data.w_count << std::endl;

        for (std::string const& s: c.data.db)
        {
            os << "db[]: " << s << std::endl;
        }

        for (auto& p: c.data.ports)
        {
            os << "ports[]: port " << p.second.num <<
               ", parser " << p.second.parser
               << std::endl;
        }

        return os;
    }
};


} // namespace td
} // namespace ys

#endif // YS_TD_CONFIG_H


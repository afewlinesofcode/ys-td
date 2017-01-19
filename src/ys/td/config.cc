/*!
 * \file
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \date   2016-08-03
 * \brief  Config class source file.
 */

#include <ys/td/config.h>
#include <iostream>

namespace ys
{
namespace td
{

/*!
 * Constructor.
 * \param argc
 * \param argv
 */
config::config(int argc, char** argv) :
    ys::config { argc, argv }
{
    /*
     * Prepare options configurable through command-line.
     */
    init_cmd_options();

    /*
     * Read options from command-line and config-file.
     */
    read_cmd_options();
    read_cfg_options();

    /*
     * Load options values into variables.
     */
    load_cfg_options();
    load_cmd_options();
}

/*!
 * Initialize command-line options.
 */
void
config::init_cmd_options()
{
    ys::config::init_cmd_options()
    ("config", option<std::string>(), "Config file path");
}

/*!
 * Read config-file options.
 */
void
config::read_cfg_options()
{
    data.cfg_path = cmd_option<std::string>("config");

    if (!data.cfg_path.empty())
    {
        ys::config::read_cfg_options<json>(data.cfg_path);
    }
}

/*!
 * Load command-line options into variables.
 */
void
config::load_cmd_options()
{
}

/*!
 * Load config-file options into variables.
 */
void
config::load_cfg_options()
{
    load_cfg_option("workers", &data.w_count);
    load_cfg_option("host", &data.host);
    load_cfg_string_list("db", &data.db);

    load_ports_cfg();
}

/*!
 * Load ports configuration into variables.
 */
void
config::load_ports_cfg()
{
    auto ports = cfg_options().get_child("ports");

    for (auto& p: ports)
    {
        int port = p.second.get<int>("num");

        data.ports.insert(
        {
            port,
            {
                port,
                p.second.get<std::string>("parser"),
                p.second.get<std::string>("typename")
            }
        });
    }
}

} // namespace td
} // namespace ys


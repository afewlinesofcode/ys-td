/*!
 * \file
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \date   2016-08-02
 * \brief  Worker class source file.
 */

#include <ys/td/worker.h>

#include <ys/logger.h>
#include <ys/td/parser.h>
#include <ys/td/st270_parser.h>

namespace ys
{
namespace td
{

/*!
 * Construct worker object.
 * \param c Application config.
 * \param s Parsed data saver.
 */
worker::worker(config const& c, saver& s) :
    /*
     * Call parent constructor.
     */
    basic_worker(),

    /*
     * Initialize handler for connection data event.
     */
    on_conn_data_
    (
        boost::bind(&worker::on_conn_data, this, _1, _2)
    ),

    /*
     * Initialize handler for connection error event.
     */
    on_conn_error_
    (
        boost::bind(&worker::on_conn_error, this, _1, _2)
    ),

    config_ { c },

    saver_ { s }

{
    /*
     * Initialize handlers for connection registration related events.
     */

    on_connection_registered(
        boost::bind(&worker::on_conn_reg, this, _1, _2)
    );

    on_connection_unregistered(
        boost::bind(&worker::on_conn_unreg, this, _1, _2)
    );
}

/*!
 * Handle new connections.
 * \param w Worker pointer.
 * \param c New connection.
 */
void
worker::on_conn_reg(ptr w, tcp_conn_ptr c)
{
    /*
     * Set handlers for connection events.
     */
    c->on_data(on_conn_data_);
    c->on_error(on_conn_error_);

    YS_LOG(debug) << "New connection registered";
}

/*!
 * Handle connection loss.
 * \param w Worker pointer.
 * \param c Lost connection.
 */
void
worker::on_conn_unreg(ptr w, tcp_conn_ptr c)
{
    sessions_.erase(c);

    YS_LOG(debug) << "Connection lost";
}

/*!
 * Handle new data on connection.
 * \param c Connection pointer.
 * \param s Size of arrived data.
 */
void
worker::on_conn_data(tcp_conn_ptr c, std::size_t s)
{
    YS_LOG(debug) << "Received " << s << " bytes";

    /*!
     * Get a parser corresponding to the connection.
     */
    parser_ptr p = get_parser(c);

    /*
     * If parser was not determined then we have nothing incommon
     * with the connection.
     */
    if (!p)
    {
        unregister_connection(c);
        return;
    }

    /*
     * Load all arrived data into the parser.
     */
    p->load(c->buffer(), s);

    /*
     * Do parsing while it's possible.
     */
    for (;;)
    {
        parser::result_type res = p->parse();

        /*
         * Regardless of the parsing result send response if there is anything
         * to send.
         */
        send_response(p, c);

        /*
         * Handle parsing result flags.
         */

        if (res.skip)
            continue;

        if (res.corrupt)
            unregister_connection(c);

        if (!res.parsed || res.corrupt)
            break;

        /*
         * If we have reached this place then send parsed data to the database.
         */
        saver_.push(p->data());
    }
}

/*!
 * Handle connection error.
 * \param c Connection pointer.
 * \param ec Error code.
 */
void
worker::on_conn_error(tcp_conn_ptr c, boost::system::error_code ec)
{
    /*
     * Remove the connection on any error.
     */

    unregister_connection(c);
}

/*!
 * Get a parser pointer associated with specified connection pointer.
 */
worker::parser_ptr
worker::get_parser(tcp_conn_ptr c)
{
    /*
     * Check whether there is open parsing session for current connection.
     */

    auto session_it = sessions_.find(c);

    if (session_it != sessions_.end())
    {
        /*
         * Session found, return corresponding parser.
         */

        return session_it->second;
    }

    /*!
     * Port on which connection was received.
     */
    auto port_num = c->socket().local_endpoint().port();

    /*!
     * Reference to ports configuration.
     */
    auto& ports = config_.data.ports;

    /*!
     * Iterator pointing to the required port.
     */
    auto port_it = ports.find(port_num);

    /*
     * Check whether the port was found.
     */
    if (port_it == ports.end())
        return nullptr;

    auto parser = create_parser(port_it->second.parser);

    /*
     * Check whether the parser was created.
     */
    if (!parser)
        return nullptr;

    /*
     * Preset tracker type name.
     */
    parser->type(port_it->second.type);

    /*
     * Save the session.
     */
    sessions_.insert({ c, parser });

    return parser;
}

/*!
 * Factory method to create parser from name.
 * \param name Parser name.
 */
worker::parser_ptr
worker::create_parser(std::string const& name) const
{
    if (name == "st270")
        return parser_ptr { new st270_parser() };

    return nullptr;
}

/*!
 * Send response from parser to the connection.
 * \param p Parser.
 * \param c Connection.
 */
void
worker::send_response(parser_ptr p, tcp_conn_ptr c) const
{
    parser::buffer_type& response = p->response();

    if (!response.empty())
    {
        c->write(response, [](boost::system::error_code const& ec, size_t n)
        {
        });

        response.clear();
    }
}

} // namespace td
} // namespace ys


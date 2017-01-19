/*!
 * \file
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \date   2016-08-02
 * \brief  Worker class header file.
 */

#ifndef YS_TD_WORKER_H
#define YS_TD_WORKER_H

#include <array>
#include <map>

#include <ys/asio/basic_worker.h>
#include <ys/td/config.h>
#include <ys/td/saver.h>
#include <ys/td/parser.h>

namespace ys
{
namespace td
{

/*!
 * Typedef for buffer allocated for each connection.
 */
using buffer_type = std::array<char, 1024>;

/*!
 * A worker class.
 */
class worker:
    public ys::asio::basic_worker<worker, buffer_type>
{
public:
    /*!
     * Parser pointer typedef.
     */
    using parser_ptr = std::shared_ptr<parser>;

    /*!
     * Construct worker object.
     * \param c Application config.
     * \param s Parsed data saver.
     */
    worker(config const& c, saver& s);

    /*!
     * Handle new connections.
     * \param w Worker pointer.
     * \param c New connection.
     */
    void
    on_conn_reg(ptr w, tcp_conn_ptr c);

    /*!
     * Handle connection loss.
     * \param w Worker pointer.
     * \param c Lost connection.
     */
    void
    on_conn_unreg(ptr w, tcp_conn_ptr c);

    /*!
     * Handle new data on connection.
     * \param c Connection pointer.
     * \param s Size of arrived data.
     */
    void
    on_conn_data(tcp_conn_ptr c, std::size_t s);

    /*!
     * Handle connection error.
     * \param c Connection pointer.
     * \param ec Error code.
     */
    void
    on_conn_error(tcp_conn_ptr c, boost::system::error_code ec);

private:
    /*!
     * A typedef for open parsing sessions.
     */
    using sessions_type = std::map<tcp_conn_ptr, parser_ptr>;

    /*!
     * Connection data handler.
     */
    tcp_conn_type::on_data_type::slot_type on_conn_data_;

    /*!
     * Connection error handler.
     */
    tcp_conn_type::on_error_type::slot_type on_conn_error_;

    /*!
     * Pointer to the application config.
     */
    config const& config_;

    /*!
     * Pointer to a parsed data saver.
     */
    saver& saver_;

    /*!
     * A map of open parser sessions.
     */
    sessions_type sessions_;

    /*!
     * Get a parser pointer associated with specified connection pointer.
     */
    parser_ptr
    get_parser(tcp_conn_ptr c);

    /*!
     * Factory method to create parser from name.
     * \param name Parser name.
     */
    parser_ptr
    create_parser(std::string const& name) const;

    /*!
     * Send response from parser to the connection
     * and clear the response buffer.
     * \param p Parser.
     * \param c Connection.
     */
    void
    send_response(parser_ptr p, tcp_conn_ptr c) const;
};

} // namespace td
} // namespace ys

#endif // YS_TD_WORKER_H


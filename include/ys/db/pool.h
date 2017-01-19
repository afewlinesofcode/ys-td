/*!
 * \file
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \date   2016-08-23
 * \brief
 */

#ifndef YS_DB_POOL_H
#define YS_DB_POOL_H

#include <pqxx/pqxx>
#include <vector>
#include <string>
#include <memory>

namespace ys
{
namespace db
{

/*!
 * Class for a pool of pqxx connections.
 */
class pool: public std::vector<std::shared_ptr<pqxx::connection>>
{
public:
    /*!
     * Connection pointer typedef.
     */
    using conn_ptr = std::shared_ptr<pqxx::connection>;

    /*!
     * Constructor.
     * \param conn_str A list of connection strings.
     */
    pool(std::vector<std::string> const& conn_str);
};

} // namespace db
} // namespace ys

#endif // YS_DB_POOL_H


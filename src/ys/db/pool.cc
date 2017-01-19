/*!
 * \file
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \date   2016-08-23
 * \brief
 */

#include <ys/db/pool.h>

namespace ys
{
namespace db
{

/*!
 * Constructor.
 * \param conn_str A list of connection strings.
 */
pool::pool(std::vector<std::string> const& conn_str)
{
    reserve(conn_str.size());

    for (auto& s: conn_str)
    {
        emplace_back(new pqxx::connection(s));
    }
}

} // namespace db
} // namespace ys


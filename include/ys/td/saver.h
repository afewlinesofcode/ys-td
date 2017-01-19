/*!
 * \file   
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \date   2016-08-29
 * \brief  
 */

#ifndef YS_TD_SAVER_H
#define YS_TD_SAVER_H

#include <vector>
#include <string>
#include <map>
#include <ys/td/parser.h>
#include <ys/blocking_queue.h>
#include <ys/db/pool.h>

namespace ys
{
namespace td
{

/*!
 * A class for updating parsed data in a database.
 */
class saver
{
public:
    /*!
     * Constructor.
     * \param db Database connections.
     */
    saver(ys::db::pool& db);

    /*!
     * Start the saver process.
     */
    void
    run();

    /*!
     * Interrupt execution.
     */
    void
    interrupt();

    /*!
     * Add data to the saver queue.
     * \param d
     */
    void
    push(parser::data_type const& d);

private:
    /*!
     * Queue typedef.
     */
    using queue_type = ys::blocking_queue<parser::data_type>;

    /*!
     * A typedef for trackers identifiers caching container.
     */
    using id_cache_type = std::map<std::string, uint32_t>;

    /*!
     * Pool of db connections.
     */
    ys::db::pool& db_;

    /*!
     * A queue of trackers data items to update.
     */
    queue_type queue_;

    /*!
     * Trackers identifiers caching container.
     */
    id_cache_type id_cache_;

    /*!
     * Save parser data in database.
     */
    void
    save(parser::data_type const& d);

    /*!
     * Get tracker id from `num` or `phone`, and `type`.
     * \param num
     * \param phone
     * \param type
     */
    uint32_t
    get_id(std::string const& num, std::string const& phone,
            std::string const& type);
};

} // namespace td
} // namespace ys

#endif // YS_TD_SAVER_H


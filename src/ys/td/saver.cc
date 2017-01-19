/*!
 * \file
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \date   2016-08-29
 * \brief
 */

#include <ys/td/saver.h>

namespace ys
{
namespace td
{

/*!
 * Constructor.
 * \param db Database connections.
 */
saver::saver(ys::db::pool& db) :
    db_ { db }
{
}

/*!
 * Start the saver process.
 */
void
saver::run()
{
    queue_type::value_type el;

    while (queue_.pop(&el))
    {
        save(el);
    }
}

/*!
 * Interrupt execution.
 */
void
saver::interrupt()
{
    queue_.interrupt();
}

/*!
 * Add data to the saver queue.
 * \param d
 */
void
saver::push(parser::data_type const& d)
{
    queue_.push(d);
}

/*!
 * Save parser data in database.
 */
void
saver::save(parser::data_type const& d)
{
    /*!
     * Tracker ID.
     */
    uint32_t id = get_id(d.num, d.phone, d.type);

    /*!
     * Suitable connection.
     */
    auto conn_ptr = db_[id % db_.size()];

    /*!
     * Transaction.
     */
    pqxx::work tx { *conn_ptr };

    tx.exec("select trackers.loginsert(" +
            tx.quote(id) + ", " +
            tx.quote(d.datetime) + ", " +
            tx.quote(d.lon) + ", " +
            tx.quote(d.lat) + ", " +
            tx.quote(static_cast<uint32_t>(d.speed)) + ", " +
            tx.quote(d.odometer) + ", " +
            tx.quote(static_cast<int32_t>(d.course)) + ", " +
            tx.quote(d.sats_glonass) + ", " +
            tx.quote(d.sats_gps) + ")"
           );

    tx.commit();
}

/*!
 * Get tracker id from `num` or `phone`, and `type`.
 * \param num
 * \param phone
 * \param type
 */
uint32_t
saver::get_id(std::string const& num, std::string const& phone,
              std::string const& type)
{
    std::string key = num + ':' + type;

    /*
     * First, try to find id in the cache.
     */

    auto id_it = id_cache_.find(key);

    if (id_it != id_cache_.end())
    {
        return id_it->second;
    }

    /*
     * Not found, take it from db.
     */

    pqxx::work tx { *db_[0] };

    uint32_t id;

    if (!num.empty())
    {
        /*
         * Search by tracker num.
         */

        auto rows = tx.exec("select id from trackers.trackers where "
                            "num = " + tx.quote(num) + " and "
                            "typeid = "
                            "(select id from trackers.types where "
                            "name = " + tx.quote(type) + ")");

        if (rows.empty())
            /*
             * No information with specified values exists.
             */
            return 0;

        /*!
         * Required tracker id.
         */
        id = rows[0]["id"].as<uint32_t>();
    }
    else if (!phone.empty())
    {
        /*
         * Search by trackers sim number.
         */

        auto rows = tx.exec("select id from trackers.trackers where "
                            "(sim = " + tx.quote(phone) +
                            " or sim2 = " + tx.quote(phone) + ") and "
                            "typeid = "
                            "(select id from trackers.types where "
                            "name = " + tx.quote(type) + ") limit 1");

        if (rows.empty())
            /*
             * No information with specified values exists.
             */
            return 0;

        /*!
         * Required tracker id.
         */
        id = rows[0]["id"].as<uint32_t>();
    }
    else
    {
        return 0;
    }

    /*
     * Save taken value in the cache.
     */

    id_cache_.insert({ key, id });

    return id;
}

} // namespace td
} // namespace ys


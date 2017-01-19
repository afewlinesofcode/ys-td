/*!
 * \file
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \date   2016-08-01
 * \brief  Trackers daemon
 */

#include <iostream>
#include <string>
#include <thread>
#include <ys/asio/simple_server.h>
#include <ys/db/pool.h>
#include <ys/td/config.h>
#include <ys/td/worker.h>
#include <ys/td/saver.h>

int
main(int argc, char* argv[])
{
    /*!
     * Application configuration.
     */
    ys::td::config conf { argc, argv };

    /*!
     * Pool of db connections.
     */
    ys::db::pool db_pool { conf.data.db };

    /*!
     * Data saver.
     */
    ys::td::saver saver { db_pool };

    /*!
     * Functor for worker initialization.
     */
    auto worker_initializer = [&conf, &saver]()
    {
        return new ys::td::worker(conf, saver);
    };

    /*!
     * Thread with running saver object.
     */
    std::thread saver_thread
    {
        [&saver]()
        {
            saver.run();
        }
    };

    /*!
     * Listening server.
     */
    ys::asio::simple_server<ys::td::worker> srv(
        /* server options */
        {
            static_cast<std::size_t>(conf.data.w_count)
        },

        worker_initializer
    );

    /*
     * Set up all required ports for listening.
     */
    for (auto& p: conf.data.ports)
    {
        srv.tcp(conf.data.host, std::to_string(p.second.num));
    }

    /*
     * Start the server (a blocking operation).
     */
    srv.run();

    /*
     * Join the saver thread.
     */
    saver.interrupt();
    saver_thread.join();

    return 0;
}


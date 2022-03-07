#ifndef AFINA_NETWORK_MT_BLOCKING_SERVER_H
#define AFINA_NETWORK_MT_BLOCKING_SERVER_H

#include <atomic>
#include <thread>

#include <afina/network/Server.h>

#include <sys/socket.h>
#include <set>
#include <condition_variable>

namespace spdlog {
class logger;
}

namespace Afina {
namespace Network {
namespace MTblocking {

/**
 * # Network resource manager implementation
 * Server that is spawning a separate thread for each connection
 */
class ServerImpl : public Server {
public:
    ServerImpl(std::shared_ptr<Afina::Storage> ps, std::shared_ptr<Logging::Service> pl,
               size_t max_treads = 5, long timeout = 5);
    ~ServerImpl();

    // Rule of three
    ServerImpl(const ServerImpl&) = delete;
    ServerImpl& operator= (const ServerImpl&) = delete;

    // See Server.h
    void Start(uint16_t port, uint32_t, uint32_t) override;

    // See Server.h
    void Stop() override;

    // See Server.h
    void Join() override;

protected:
    // Method is running in the connection acceptor thread
    void OnRun();

    void Worker(int client_socket, sockaddr client_addr);

private:
    // Logger instance
    std::shared_ptr<spdlog::logger> _logger;



    // Atomic flag to notify threads when it is time to stop. Note that
    // flag must be atomic in order to safely publisj changes cross thread
    // bounds
    std::atomic<bool> running;

    //  Number of threads limiting
    std::atomic<size_t> thread_counter;
    const size_t max_treads;

    // Server socket to accept connections on
    int _server_socket;

    // Read timeout
    struct timeval tv;

    // Thread to run network on
    std::thread _thread;

    // Set for stopping worker threads
    std::set<int> _worker_sockets;
    std::mutex _stop_mutex;

    // For joining server thread
    std::mutex _join_mutex;
    std::condition_variable _join_cv;
};

} // namespace MTblocking
} // namespace Network
} // namespace Afina

#endif // AFINA_NETWORK_MT_BLOCKING_SERVER_H

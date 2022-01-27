#ifndef SRC_SERVER_H
#define SRC_SERVER_H

#include <list>
#include <memory>
#include <string_view>
#include <thread>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include <compose.h>
#include <config.h>

class server
{
private:
    boost::asio::io_context io_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::thread worker_;
    const std::string root_;

public:
    server(std::string_view root, config& the_config, compose& the_compose);
    ~server();

private:
    void accept();
    void on_accept(const boost::system::error_code& error,
        boost::asio::ip::tcp::socket socket);
};

#endif // SRC_SERVER_H

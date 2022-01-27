#include <fstream>
#include <iostream>

#include "session.h"

#include "server.h"

server::server(
    std::string_view root, config& the_config, compose& the_compose) :
    acceptor_{ io_ },
    root_{ root }
{
    boost::asio::ip::tcp::endpoint pub_endpoint
    {
        boost::asio::ip::make_address_v4("0.0.0.0"), static_cast<unsigned short>
        (the_config["port"].get_long())
    };
    acceptor_.open(pub_endpoint.protocol());
    acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
    acceptor_.bind(pub_endpoint);
    acceptor_.listen(boost::asio::socket_base::max_listen_connections);
    io_.dispatch(std::bind(&server::accept, this));
    worker_ = std::thread
    {
        [&]()
        {
            try
            {
                io_.run();
            }
            catch (const std::exception& e)
            {
                the_compose.request_stop();
            }
        }
    };
}

server::~server()
{
    io_.stop();
    if (worker_.joinable())
    {
        worker_.join();
    }
}

void server::accept()
{
    acceptor_.async_accept(std::bind(&server::on_accept, this,
        std::placeholders::_1, std::placeholders::_2));
}

void server::on_accept(
    const boost::system::error_code& error, boost::asio::ip::tcp::socket socket)
{
    std::make_shared<session>(root_, std::move(socket))->run();
    accept();
}

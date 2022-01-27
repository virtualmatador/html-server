#ifndef SRC_SESSION_H
#define SRC_SESSION_H

#include <map>
#include <memory>
#include <set>
#include <string>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

class session : public std::enable_shared_from_this<session>
{
private:
    boost::beast::tcp_stream stream_;
    boost::beast::flat_buffer buffer_;
    boost::beast::http::request<boost::beast::http::string_body> req_;
    std::shared_ptr<void> res_;
    const std::string& root_;

public:
    std::string id_;
    std::string target_;
    std::map<std::string, std::string> params_;
    std::string title_;
    std::string body_;
    std::set<std::string> assets_;

public:
    session(const std::string& root, boost::asio::ip::tcp::socket&& socket);
    void run();

private:
    void read();
    void on_read(const boost::system::error_code& error,
        std::size_t bytes_transferred);
    void on_write(bool close, boost::beast::error_code ec,
        std::size_t bytes_transferred);
    template<class BODY>
    void write_body(boost::beast::http::response<BODY>&& res);

private:
    static std::map<std::string, std::string> mime_types_;
};

void render(session* the_session);

#endif // SRC_SESSION_H

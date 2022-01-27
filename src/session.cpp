#include <regex>

#include "session.h"

std::map<std::string, std::string> session::mime_types_ =
{
    { "htm", "text/html" },
    { "html", "text/html" },
    { "css", "text/css" },
    { "txt", "text/plain" },
    { "js", "application/javascript" },
    { "json", "application/json" },
    { "xml", "application/xml" },
    { "swf", "application/x-shockwave-flash" },
    { "flv", "video/x-flv" },
    { "png", "image/png" },
    { "jpe", "image/jpeg" },
    { "jpeg", "image/jpeg" },
    { "jpg", "image/jpeg" },
    { "gif", "image/gif" },
    { "bmp", "image/bmp" },
    { "ico", "image/vnd.microsoft.icon" },
    { "tiff", "image/tiff" },
    { "tif", "image/tiff" },
    { "svg", "image/svg+xml" },
    { "svgz", "image/svg+xml" },
    { "wasm", "application/wasm" },
};

session::session(const std::string& root,
    boost::asio::ip::tcp::socket&& socket) :
    stream_{ std::move(socket) },
    root_{ root }
{
}

void session::run()
{
    boost::asio::dispatch(boost::beast::bind_front_handler(
        &session::read, shared_from_this()));
}

void session::read()
{
    req_ = {};
    boost::beast::http::async_read(stream_, buffer_, req_,
        boost::beast::bind_front_handler(
            &session::on_read, shared_from_this()));
    boost::beast::get_lowest_layer(stream_).expires_after(
        std::chrono::seconds(30));
}

void session::on_read(
    const boost::beast::error_code& error, std::size_t bytes_transferred)
{
    if (!error)
    {
        target_ = std::string { req_.target().data(), req_.target().size() };
        std::smatch extension;
        auto mime = mime_types_.end();
        if (target_.size() > 0 && target_.back() == '/')
        {
            boost::beast::http::response<boost::beast::http::string_body> res;
            res.result(boost::beast::http::status::ok);
            title_.clear();
            body_.clear();
            assets_.clear();
            render(this);
            std::string import_assets;
            for (const auto& asset : assets_)
            {
                import_assets += R"EOF(    <link id=')EOF" + asset +
    R"EOF(.css' rel='stylesheet' href='/)EOF" +
            asset + R"EOF(.css'>
    <script id=')EOF" + asset + R"EOF(.js' src='/)EOF" + asset +
            R"EOF(.js'></script>
)EOF";
            }
            res.body() = R"EOF(
<!DOCTYPE html>
<html>
<head>
    <meta charset='utf-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <script src='/wasm.js'></script>
    <link rel='stylesheet' href='/index.css'>
    <script src='/index.js'></script>
)EOF" + import_assets +
R"EOF(    <title>)EOF" + std::move(title_) + R"EOF(</title>
</head>
<body>
)EOF" + std::move(body_) + R"EOF(</body>
</html>
)EOF";
            // TODO add session cookie
            write_body(std::move(res));
        }
        else if (std::regex_match(target_, extension,
            std::regex(".*\\.([^\\./]+)")) && extension.size() == 2 &&
            (mime = mime_types_.find(extension[1])) != mime_types_.end())
        {
            boost::beast::http::response<boost::beast::http::file_body> res;
            boost::beast::error_code ec;
            res.body().open((root_ + target_).c_str(),
                boost::beast::file_mode::scan, ec);
            if (!ec)
            {
                res.result(boost::beast::http::status::ok);
                res.set(boost::beast::http::field::content_type,
                    mime->second.c_str());
                res.content_length(res.body().size());
                write_body(std::move(res));
            }
            else
            {
                boost::beast::http::response<boost::beast::http::empty_body>
                    res;
                res.result(boost::beast::http::status::bad_request);
                write_body(std::move(res));
            }
        }
        else
        {
            boost::beast::http::response<boost::beast::http::empty_body> res;
            res.result(boost::beast::http::status::bad_request);
            write_body(std::move(res));
        }
        boost::beast::get_lowest_layer(stream_).expires_after(
            std::chrono::seconds(30));
    }
}

template<class BODY>
void session::write_body(boost::beast::http::response<BODY>&& res)
{
    res.version(req_.version());
    res.keep_alive(res.result() == boost::beast::http::status::ok ?
        req_.keep_alive() : false);
    auto s_res =
        std::make_shared<boost::beast::http::response<BODY>>(std::move(res));
    res_ = s_res;
    boost::beast::http::async_write(stream_, *s_res,
        boost::beast::bind_front_handler(
            &session::on_write, shared_from_this(), res.need_eof()));
}

void session::on_write(bool close,
    boost::beast::error_code ec, std::size_t bytes_transferred)
{
    res_ = nullptr;
    if(!ec)
    {
        if(!close)
        {
            read();
            return;
        }
        stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send);
    }
}

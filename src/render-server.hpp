#ifndef SRC_RENDER_SERVER_HPP
#define SRC_RENDER_SERVER_HPP

#include <sstream>

#include "render.hpp"

template <class T, class ...P>
std::string server_side(SESSION_TYPE the_session, P... v)
{
#ifndef CLIENT_SIDE
    the_session->assets_.insert(get_name<T>());
#endif
    return T::template render<true>(the_session, std::forward<P>(v)...);
}

#endif // SRC_RENDER_SERVER_HPP

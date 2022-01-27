#ifndef SRC_RENDER_HPP
#define SRC_RENDER_HPP

#include <sstream>
#include <string>

#ifndef CLIENT_SIDE
#include "session.h"
template <class T>
std::string get_name();
#endif

template <class T, class ...P>
std::string server_side(SESSION_TYPE the_session, P... v)
{
#ifndef CLIENT_SIDE
    the_session->assets_.insert(get_name<T>());
#endif
    return T::render(the_session, std::forward<P>(v)...);
}

#ifndef CLIENT_SIDE
template <class T, class ...P>
std::string client_side(SESSION_TYPE the_session, P... v)
{
    std::ostringstream params;
    params << "'" << get_name<T>() << "', '" << the_session->id_ << "'";
    ((params << ", " << std::forward<P>(v)), ...);
    return R"EOF(<script>
    Module['lazyLoad']()EOF" + params.str() + R"EOF();
</script>
)EOF";
}

// Auto Generated
#include "name.hpp"
#endif

#endif // SRC_RENDER_HPP

#ifndef SRC_RENDER_H
#define SRC_RENDER_H

#ifndef CLIENT_SIDE
#include "name.hpp"
#include "session.h"
using SESSION_TYPE  = session*;
#else
#include <string>
using SESSION_TYPE  = std::string;
#endif

template <class T, class ...P>
std::string client_side(SESSION_TYPE the_session, P... v)
{
#ifndef CLIENT_SIDE
    std::ostringstream params;
    params << "'" << get_name<T>() << "', '" << the_session->id_ << "'";
    ((params << ", " << std::forward<P>(v)), ...);
    return R"EOF(<script>
    Module['lazyLoad']()EOF" + params.str() + R"EOF();
</script>
)EOF";
#else
    return "";
#endif
}

#endif // SRC_RENDER_H

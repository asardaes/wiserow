#ifndef WISEROW_VISITORS_H_
#define WISEROW_VISITORS_H_

#include <boost/variant/static_visitor.hpp>

namespace wiserow {

// =================================================================================================

template<typename T>
class GenericIdentityVisitor : public boost::static_visitor<T>
{
public:
    GenericIdentityVisitor() {}

    template<typename U>
    T operator()(const U val) const {
        return val;
    }
};

} // namespace wiserow

#endif // WISEROW_VISITORS_H_

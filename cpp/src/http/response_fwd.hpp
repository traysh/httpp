#pragma once

namespace HTTP {

class Connection;
template<class ConnectionType>
struct ResponseT;
using Response = ResponseT<Connection>;

}

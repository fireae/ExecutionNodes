#pragma once
#include <cstdint>

namespace execution_nodes {

typedef size_t ConnectionId;

static constexpr ConnectionId no_connection {static_cast<ConnectionId>(0)};

}
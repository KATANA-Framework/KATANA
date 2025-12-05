#include "katana/core/handler_context.hpp"

namespace katana::http {

// Thread-local storage definitions
thread_local const request* handler_context::current_request_ = nullptr;
thread_local request_context* handler_context::current_context_ = nullptr;

} // namespace katana::http

#include "call_finalizer.h"

namespace grpc {

std::set<std::shared_ptr<CallFinalizer>> CallFinalizer::queue_ = {};
std::mutex CallFinalizer::queue_mutex_ {};

}

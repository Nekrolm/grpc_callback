#pragma once

#include "simplified_async_callback_api.h"

namespace grpc {

class SimpleAsyncClient
{
public:
    SimpleAsyncClient()
    {
        worker_ = make_queue_worker(&queue_);
    }

    ~SimpleAsyncClient()
    {
        queue_.Shutdown();

        if (worker_.joinable())
            worker_.join();
    }

    template<class RequestT, class ResponceT, class StubT>
    void request(StubT* stub, pointer_to_request_call_t<RequestT, ResponceT, StubT> request_ptr,
                 const RequestT& message,
                 responce_handler_t<ResponceT> callback = [](...) {})
    {
        async_unary_call(make_request_call(stub, request_ptr), message, &queue_, std::move(callback));
    }

    template<class RequestT, class ResponceT, class StubT>
    void request(StubT* stub, pointer_to_request_call_t<RequestT, ResponceT, StubT> request_ptr,
                 const RequestT& message,
                 simple_responce_handler_t<ResponceT> callback = [](...) {})
    {
        async_unary_call(make_request_call(stub, request_ptr), message, &queue_, std::move(callback));
    }

private:
    std::thread worker_;
    grpc::CompletionQueue queue_;
};

}

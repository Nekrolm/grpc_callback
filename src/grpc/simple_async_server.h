#ifndef SIMPLE_ASYNC_SERVER_H
#define SIMPLE_ASYNC_SERVER_H

#include "simplified_async_callback_api.h"

#include <grpcpp/server.h>

#include <thread>

namespace grpc {

class SimpleAsyncServer
{
public:
    SimpleAsyncServer(std::unique_ptr<grpc::Server> server,
                      std::unique_ptr<grpc::ServerCompletionQueue> cq) :
        cq_(std::move(cq)),
        server_(std::move(server))
    {
        worker_thread_ = make_queue_worker(cq_.get());
    }

    ~SimpleAsyncServer()
    {
        server_->Shutdown();
        cq_->Shutdown();

        if (worker_thread_.joinable())
            worker_thread_.join();
    }

    template<class RequestT, class ResponceT, class Service>
    void allow_requests(Service* service,
                        pointer_to_request_registration_call_t<RequestT, ResponceT, Service> registration,
                        request_handler_t<RequestT, ResponceT> handler)
    {
        grpc::allow_requests(grpc::make_request_registration_call(service, registration), cq_.get(), std::move(handler));
    }

    template<class RequestT, class ResponceT, class Service>
    void allow_requests(Service* service,
                        pointer_to_request_registration_call_t<RequestT, ResponceT, Service> registration,
                        simple_request_handler_t<RequestT, ResponceT> handler)
    {
        grpc::allow_requests(grpc::make_request_registration_call(service, registration), cq_.get(), std::move(handler));
    }

private:
    std::thread worker_thread_;
    std::unique_ptr<grpc::ServerCompletionQueue> cq_;
    std::unique_ptr<grpc::Server> server_;
};

}
#endif // SIMPLE_ASYNC_SERVER_H

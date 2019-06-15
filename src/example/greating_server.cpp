#include "greating_server.h"

#include <QDebug>

#include <thread>

#include <grpcpp/server_builder.h>


#include <grpc/simplified_async_callback_api.h>


GreatingServer::GreatingServer(const std::string& host)
{
    grpc::ServerBuilder builder;

    builder.AddListeningPort(host, grpc::InsecureServerCredentials());
    builder.RegisterService(&service_);
    auto cq     = builder.AddCompletionQueue();
    auto server = builder.BuildAndStart();

    server_ = std::make_unique<grpc::SimpleAsyncServer>(std::move(server), std::move(cq));

    grpc::simple_request_handler_t<test::HelloRequest,
                                   test::HelloReply> handler =
        [](std::shared_ptr<test::HelloRequest> request, test::HelloReply* reply) {
            reply->set_message("Hello, " + request->name());
            return grpc::Status::OK;
        };


    server_->allow_requests(&service_, &test::Greeting::AsyncService::RequestSayHello, handler);
}

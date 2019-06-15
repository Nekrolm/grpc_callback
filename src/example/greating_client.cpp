#include "greating_client.h"

#include <grpcpp/channel.h>

#include <QDebug>
#include <functional>
#include <thread>

#include <grpc/simplified_async_callback_api.h>

GreatingClient::GreatingClient(std::shared_ptr<grpc::Channel> ch) :
    stub_(test::Greeting::NewStub(ch))
{}

GreatingClient::~GreatingClient()
{}

void GreatingClient::greeting(const std::string& name)
{
    test::HelloRequest request;

    request.set_name(name);

    grpc::simple_responce_handler_t<test::HelloReply> callback = [](std::shared_ptr<test::HelloReply> reply,
                                                                    std::shared_ptr<grpc::Status> status) {
                                                                     if (status->ok())
                                                                         std::cout << reply->message() << std::endl;
                                                                     else
                                                                         std::cout << "failed!" << std::endl;
                                                                 };

    client_.request(stub_.get(), &test::Greeting::Stub::AsyncSayHello, request, callback);
}

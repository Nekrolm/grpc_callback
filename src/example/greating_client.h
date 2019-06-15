#ifndef GREATING_CLIENT_H
#define GREATING_CLIENT_H

#include "protos/test.grpc.pb.h"

#include <grpcpp/completion_queue.h>

#include <string>
#include <thread>

#include <grpc/simple_async_client.h>


class GreatingClient
{
public:
    GreatingClient(std::shared_ptr<grpc::Channel> ch);
    ~GreatingClient();

    void greeting(const std::string& name);

private:
    std::unique_ptr<test::Greeting::Stub> stub_;

    grpc::SimpleAsyncClient client_;
};
#endif // GREATING_CLIENT_H

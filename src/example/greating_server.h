#ifndef GREATING_SERVER_H
#define GREATING_SERVER_H

#include <protos/test.grpc.pb.h>

#include <grpcpp/server.h>

#include <thread>

#include <grpc/simple_async_server.h>

class GreatingServer : public std::enable_shared_from_this<GreatingServer>
{
public:
    GreatingServer(const std::string& host);

private:

    test::Greeting::AsyncService service_;
    std::unique_ptr<grpc::SimpleAsyncServer> server_;
};
#endif // GREATING_SERVER_H

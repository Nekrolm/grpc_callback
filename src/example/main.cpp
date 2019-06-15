#include <thread>

#include "greating_server.h"
#include "greating_client.h"


#include <grpcpp/server.h>
#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/security/server_credentials.h>



int main(int argc, char* argv[])
{
    auto server = std::make_shared<GreatingServer>(std::string("0.0.0.0:10055"));


    std::this_thread::sleep_for(std::chrono::milliseconds(5000));


    GreatingClient client(grpc::CreateChannel("localhost:10055",
                                              grpc::InsecureChannelCredentials()));


    for (int i = 0; i < 5; ++i)
    {
        client.greeting("World" + std::to_string(i));
    }



    GreatingClient client2(grpc::CreateChannel("localhost:10055",
                                               grpc::InsecureChannelCredentials()));


    for (int i = 0; i < 5; ++i)
    {
        client2.greeting("Peace" + std::to_string(i));
    }


    std::this_thread::sleep_for(std::chrono::milliseconds(5000));


    return 0;
}

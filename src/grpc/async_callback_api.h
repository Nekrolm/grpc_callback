#ifndef CALLBACK_API_H
#define CALLBACK_API_H

#include <grpcpp/client_context.h>
#include <grpcpp/completion_queue.h>
#include <grpcpp/server.h>

#include "call_finalizer.h"

namespace grpc {

template<class ResponceT>
using responce_handler_t = std::function<void (std::shared_ptr<grpc::ClientContext>,
                                               std::shared_ptr<ResponceT>,
                                               std::shared_ptr<grpc::Status>)>;

template<class ResponceT>
using responce_reader_t =  std::unique_ptr<grpc::ClientAsyncResponseReader<ResponceT>>;

template<class RequestT, class ResponceT>
using request_call_t = std::function<responce_reader_t<ResponceT>(grpc::ClientContext*,
                                                                  const RequestT&,
                                                                  grpc::CompletionQueue*)>;

template<class RequestT, class ResponceT, class StubT>
using pointer_to_request_call_t = responce_reader_t<ResponceT>(StubT::*)(grpc::ClientContext*,
                                                                         const RequestT&,
                                                                         grpc::CompletionQueue*);



template<class RequestT, class ResponceT, class StubT>
request_call_t<RequestT, ResponceT> make_request_call(StubT* stub, pointer_to_request_call_t<RequestT, ResponceT, StubT> method)
{
    return [stub, method](auto&& ... args) { return (stub->*method)(std::forward<decltype (args)>(args)...); };
}

template<class RequestT, class ResponceT>
inline void perform_async_unary_call(request_call_t<RequestT, ResponceT> call,
                                     std::shared_ptr<grpc::ClientContext> context,
                                     const RequestT& request,
                                     grpc::CompletionQueue* cq,
                                     responce_handler_t<ResponceT> callback)
{
    auto status   = std::make_shared<grpc::Status>();
    auto responce = std::make_shared<ResponceT>();

    auto finalizer = grpc::CallFinalizer::make([context, responce, status, cbk = std::move(callback)] {
                                                   cbk(context, responce, status);
                                               });

    auto reader = call(context.get(), request, cq);

    reader->Finish(responce.get(), status.get(), finalizer.get());
}

inline std::thread make_queue_worker(grpc::CompletionQueue* queue)
{
    return std::thread([queue] {
                           void* finalizer = nullptr;
                           bool ok         = false;

                           while (queue->Next(&finalizer, &ok))
                               if (ok)
                                   static_cast<grpc::CallFinalizer*>(finalizer)->process();
                       });
}

template<class ResponceT>
using responder_t = grpc::ServerAsyncResponseWriter<ResponceT>;

template<class RequsetT, class ResponceT>
using request_registration_call_t = std::function<void (grpc::ServerContext*,
                                                        RequsetT*,
                                                        responder_t<ResponceT>*,
                                                        grpc::CompletionQueue*,
                                                        grpc::ServerCompletionQueue*,
                                                        void*)>;



template<class RequestT, class ResponceT, class Service>
using pointer_to_request_registration_call_t = void (Service::*)(grpc::ServerContext*,
                                                                 RequestT*,
                                                                 responder_t<ResponceT>*,
                                                                 grpc::CompletionQueue*,
                                                                 grpc::ServerCompletionQueue*,
                                                                 void*);


template<class RequestT, class ResponceT, class Service>
request_registration_call_t<RequestT, ResponceT> make_request_registration_call(Service* service,
                                                                                pointer_to_request_registration_call_t<RequestT, ResponceT,
                                                                                                                       Service> method)
{
    return [service, method](auto&& ... args) { (service->*method)(std::forward<decltype(args)>(args)...); };
}

template<class RequestT, class ResponceT>
using request_handler_t = std::function<grpc::Status(std::shared_ptr<grpc::ServerContext>, std::shared_ptr<RequestT>, ResponceT*)>;


using request_processed_signal_handler_t = std::function<void (std::shared_ptr<grpc::ServerContext>)>;


template<class RequsetT, class ResponceT>
inline void prepare_server_request_placeholder(request_registration_call_t<RequsetT, ResponceT> call,
                                               std::shared_ptr<grpc::ServerContext> context,
                                               grpc::CompletionQueue* completion_queue,
                                               grpc::ServerCompletionQueue* server_completion_queue,
                                               request_handler_t<RequsetT, ResponceT> handler,
                                               request_processed_signal_handler_t on_processed = [](...) {})
{
    auto request   = std::make_shared<RequsetT>();
    auto responder = std::make_shared<responder_t<ResponceT>>(context.get());

    auto finalizer = grpc::CallFinalizer::make([context, request, responder, cbk = std::move(handler),
                                                on_processed = std::move(on_processed)] {
                                                   ResponceT responce;
                                                   auto status                 = cbk(context, request, &responce);
                                                   auto finalizer_on_processed =
                                                       grpc::CallFinalizer::make([responder, context,
                                                                                  on_processed = std::move(on_processed)] {
                                                                                     on_processed(context);
                                                                                 });
                                                   responder->Finish(responce, status, finalizer_on_processed.get());
                                               });

    call(context.get(), request.get(), responder.get(), completion_queue, server_completion_queue, finalizer.get());
}

template<class RequestT, class ResponceT>
inline void prepare_server_request_placeholder(request_registration_call_t<RequestT, ResponceT> call,
                                               std::shared_ptr<grpc::ServerContext> context,
                                               grpc::ServerCompletionQueue* completion_queue,
                                               request_handler_t<RequestT, ResponceT> handler)
{
    prepare_server_request_placeholder(std::move(call), context, completion_queue, completion_queue, std::move(handler));
}

}
#endif // CALLBACK_API_H

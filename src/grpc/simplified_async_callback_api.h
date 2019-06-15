#ifndef SIMPLIFIED_ASYNC_CALLBACK_API_H
#define SIMPLIFIED_ASYNC_CALLBACK_API_H

#include "async_callback_api.h"


namespace grpc {

template<class RequestT, class ResponceT>
using prepare_handler_t = std::function<void (request_handler_t<RequestT, ResponceT>)>;


template<class RequestT, class ResponceT>
using simple_request_handler_t = std::function<grpc::Status(std::shared_ptr<RequestT>, ResponceT*)>;

template<class ResponceT>
using simple_responce_handler_t = std::function<void (std::shared_ptr<ResponceT>,
                                                      std::shared_ptr<grpc::Status>)>;


namespace impl {

template<class RequestT, class ResponceT>
inline request_handler_t<RequestT, ResponceT> make_request_handler(simple_request_handler_t<RequestT, ResponceT> simple_handler)
{
    return [handler = std::move(simple_handler)](auto&& ctx, auto&& ... args) { return handler(std::forward<decltype (args)>(args)...); };
}

template<class ResponceT>
inline responce_handler_t<ResponceT> make_responce_handler(simple_responce_handler_t<ResponceT> simple_handler)
{
    return [handler = std::move(simple_handler)](auto&& ctx, auto&& ... args) { return handler(std::forward<decltype (args)>(args)...); };
}

template<class RequestT, class ResponceT>
inline request_handler_t<RequestT, ResponceT> make_self_registrate_handler(prepare_handler_t<RequestT, ResponceT> prepare,
                                                                           request_handler_t<RequestT, ResponceT> handler)
{
    return [handler = std::move(handler), prepare = std::move(prepare)](auto&& ... args) {
               prepare(make_self_registrate_handler(prepare, handler));
               return handler(std::forward<decltype (args)>(args) ...);
           };
}

}




template<class RequestT, class ResponceT>
inline void allow_requests(request_registration_call_t<RequestT, ResponceT> call,
                           grpc::ServerCompletionQueue* queue,
                           request_handler_t<RequestT, ResponceT> request_handler)
{
    prepare_handler_t<RequestT, ResponceT> prepare_request = [call = std::move(call),
                                                              queue](request_handler_t<RequestT, ResponceT> handler) {
                                                                 auto context = std::make_shared<grpc::ServerContext>();

                                                                 prepare_server_request_placeholder(call,
                                                                                                    context,
                                                                                                    queue,
                                                                                                    queue,
                                                                                                    std::move(handler));
                                                             };


    prepare_request(impl::make_self_registrate_handler(prepare_request, std::move(request_handler)));
}

template<class RequestT, class ResponceT>
inline void allow_requests(request_registration_call_t<RequestT, ResponceT> call,
                           grpc::ServerCompletionQueue* queue,
                           simple_request_handler_t<RequestT, ResponceT> request_handler)
{
    allow_requests(std::move(call), queue, impl::make_request_handler(std::move(request_handler)));
}

template<class RequestT, class ResponceT>
inline void async_unary_call(request_call_t<RequestT, ResponceT> call, const RequestT& request, grpc::CompletionQueue* queue,
                             responce_handler_t<ResponceT> handler = [](...) {})
{
    auto context = std::make_shared<grpc::ClientContext>();

    perform_async_unary_call(std::move(call), context, request, queue, std::move(handler));
}

template<class RequestT, class ResponceT>
inline void async_unary_call(request_call_t<RequestT, ResponceT> call, const RequestT& request, grpc::CompletionQueue* queue,
                             simple_responce_handler_t<ResponceT> handler = [](...) {})
{
    async_unary_call(std::move(call), request, queue, impl::make_responce_handler(std::move(handler)));
}

}
#endif // SIMPLIFIED_ASYNC_CALLBACK_API_H

#ifndef CALLFINALIZER_H
#define CALLFINALIZER_H

#include <memory>
#include <functional>
#include <mutex>
#include <set>
#include <thread>

namespace grpc {

class CallFinalizer : public std::enable_shared_from_this<CallFinalizer>
{
public:

    using callback_t = std::function<void (void)>;


    static std::shared_ptr<CallFinalizer> make(callback_t cbk)
    {
        auto ret = std::shared_ptr<CallFinalizer>(new CallFinalizer);

        ret->cmd_ = [cbk = std::move(cbk)] { cbk(); };

        std::lock_guard<std::mutex> lock(queue_mutex_);
        queue_.insert(ret);

        return ret;
    }

    ~CallFinalizer() = default;

    void process()
    {
        cmd_();
        std::lock_guard<std::mutex> lock(queue_mutex_);
        queue_.erase(shared_from_this());
    }

private:

    inline static std::mutex queue_mutex_;
    inline static std::set<std::shared_ptr<CallFinalizer>> queue_;

    callback_t cmd_;
};

}
#endif // CALLFINALIZER_H

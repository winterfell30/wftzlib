#pragma once
#include <memory>
#include <queue>
#include <mutex>
#include <atomic>

namespace wftz {

/*
* 双buffer 单线程写，多线程读
* 主buffer处理结束时触发buffer指针交换
*/
template <typename T>
class DoubleBuffer
{
public:
    using value_type = T;
    using buffer_type = ::std::queue<T>;

    DoubleBuffer() : _idx(0)
    {
        for (size_t i = 0; i < 2; ++i)
        {
            _ptr.push_back(std::make_shared<buffer_type>());
            _mutex.push_back(std::make_shared<std::mutex>());
        }
    }
    virtual ~DoubleBuffer() = default;

    void push(const value_type& data)
    {
        const auto cur_idx = _idx.load();
        const auto bak_idx = _get_nxt_idx(cur_idx);
        auto& mutex_ = _mutex[bak_idx];
        auto& ptr_ = _ptr[bak_idx];

        std::lock_guard<std::mutex> lock(*mutex_);
        ptr_->push(data);
    }

    bool pop(value_type& data)
    {
        if (this->empty())
        {
            this->_swap_ptr();
        }

        const auto cur_idx = _idx.load();
        auto& mutex_ = _mutex[cur_idx];
        auto& ptr_ = _ptr[cur_idx];

        std::lock_guard<std::mutex> lock(*mutex_);
        
        if (ptr_->empty())
        {
            return false;
        }

        data = ptr_->front();
        ptr_->pop();

        return true;
    }

    size_t size()
    {
        const auto cur_idx = _idx.load();
        auto& mutex_ = _mutex[cur_idx];
        auto& ptr_ = _ptr[cur_idx];

        std::lock_guard<std::mutex> lock(*mutex_);
        return ptr_->size();
    }

    bool empty()
    {
        const auto cur_idx = _idx.load();
        auto& mutex_ = _mutex[cur_idx];
        auto& ptr_ = _ptr[cur_idx];

        std::lock_guard<std::mutex> lock(*mutex_);
        return ptr_->empty();
    }

private:
    int _get_nxt_idx(int cur_idx)
    {
        return cur_idx ^ 1;
    }

    void _swap_ptr()            
    {
        const auto cur_idx = _idx.load();
        const auto bak_idx = _get_nxt_idx(cur_idx);
        auto& mutex_ = _mutex[bak_idx];

        std::lock_guard<std::mutex> lock(*mutex_);
        _idx.store(bak_idx);
    }

    std::vector<std::shared_ptr<buffer_type> > _ptr;
    std::atomic<int> _idx;
    std::vector<std::shared_ptr<std::mutex> > _mutex;
};

}
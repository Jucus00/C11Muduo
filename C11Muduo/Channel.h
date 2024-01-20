#pragma once

#include "noncopyable.h"
#include "Timestamp.h"

#include <functional>
#include <memory>

class EventLoop;

/*
    理清楚 EventLoop、Channel、Poller之间的关系 <= 对应Reactor模型上的事件多路分发器
    Channel 理解为通道，封装了sockfd和其感兴趣的event，如EPOLLIN、EPOLLOUT事件
    还绑定了poller返回的具体事件
*/
class Channel : noncopyable
{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop *loop, int fd);
    ~Channel();

    //fd得到poller通知以后，处理事件的回调函数
    void handleEvent(Timestamp receiveTime);  

    // 设置回调函数对象
    void setReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }
    void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
    void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
    void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

    //主要是防止channel被手动remove掉，channel还在执行回调操作，监听channel的状态
    void tie(const std::shared_ptr<void>&);

    int fd() const { return fd_; }
    int events() const { return events_; }

    //供epoll事件设置的接口
    int set_revents(int revt) { revents_ = revt; }

    // 设置fd相应的事件状态
    ////update()调用epoll_ctl，设置感兴趣事件
    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ = kNoneEvent; update(); }

    //返回fd当前事件状态
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }

    int index() { return index_; }
    void set_index(int idx) { index_ = idx; }

    //返回Channel所属的事件循环 其实就是所属线程
    EventLoop* ownerLoop() { return loop_; }
    void remove();
private:

    void update();
    void handleEventWithGuard(Timestamp receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_; //事件循环
    const int fd_;    //fd,poller监听的对象
    int events_;      //注册fd感兴趣的事件
    int revents_;     //poller返回的具体发生的事件
    int index_;

    //观察一个强指针智能，监听当前Channel状态
    std::weak_ptr<void> tie_;
    bool tied_;

    /*
    因为channel通道能够获知fd最终发生的具体事件
    所以它负责调用具体的回调操作
    */
    ReadEventCallback readCallback_;  //回调函数
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};


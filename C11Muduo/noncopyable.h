#pragma once

/*
   派生类可以进行正常构造与析构，但派生类无法进行拷贝与复制操作
*/
class noncopyable
{
public:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};
#include "Buffer.h"

#include <errno.h>
#include <sys/uio.h>
#include <unistd.h>

/*
    从fd上读取数据，Poller工作在LT模式上
    Buffer缓冲区是有大小的，但是从fd上读取数据的时候，但不知道tcp数据的最终大小

*/
ssize_t Buffer::readFd(int fd, int* saveErrno)
{
    char extrabuf[65536] = {0}; //栈上的内存空间 64K
    
    struct iovec vec[2] = { 0 };
    
    const size_t writable = writableBytes(); //这是Buffer底层缓冲区能写的空间大小 
    vec[0].iov_base = begin() + writerIndex_;
    vec[0].iov_len = writable;

    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;
    
    //iovcnt：几个缓冲区
    const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;


    const ssize_t n = ::readv(fd, vec, iovcnt);
    if (n < 0)
    {
        *saveErrno = errno;
    }
    else if (n <= writable) //Buffer缓冲区已经足够缓冲已经读出来的数据
    {
        writerIndex_ += n;
    }
    else //extrabuf里面也写入了数据
    {
        writerIndex_ = buffer_.size();
        append(extrabuf, n - writable);  // writerIndex_开始写 n - writable大小的数据
    }

    return n;
}

ssize_t Buffer::writeFd(int fd, int* saveErrno)
{
    ssize_t n = ::write(fd, peek(), readableBytes());
    if (n < 0)
    {
        *saveErrno = errno;
    }
    return n;
}
#include <C11Muduo/TcpServer.h>
#include <C11Muduo/Logger.h>

class EchoServer{

public:
    EchoServer(EventLoop* loop,const InetAddress& addr,const std::string& name)
    :server_(loop,addr,name)
    ,loop_(loop){

        server_.setConnectionCallback(std::bind(&EchoServer::onConnection,this,std::placeholders::_1));

        server_.setMessageCallback(std::bind(&EchoServer::onMessage
        ,this
        ,std::placeholders::_1
        ,std::placeholders::_2
        ,std::placeholders::_3));

        //设置合适的subloop线程数量。
        server_.setThreadNum(3);
    }

    void start(){
        server_.start();
        loop_->loop(); //启动baseLoop事件循环
    }

private:
    void onConnection(const TcpConnectionPtr& conn){
        if (conn->connected()){
            LOG_INFO("Connection IP: %s 成功连接服务器", conn->peerAddress().toIpPort().c_str());
        }
        else{
            LOG_INFO("Connection IP: %s 与服务器断开连接", conn->peerAddress().toIpPort().c_str());
        }
    }

    void onMessage(const TcpConnectionPtr& conn,Buffer* buf,Timestamp time){
        std::string msg = buf->retrieveAllAsString();
        conn->send(msg);
    }

private:
    TcpServer server_;
    EventLoop* loop_;

};


int main(void){
    EventLoop baseLoop;

    //InetAddress addr(7777, "172.19.7.134"); //可监听指定地址
    InetAddress addr(7777); //默认监听本地环回地址。
    // struct sockaddr_in server_addr;
    // server_addr.sin_family = AF_INET;                // 选择协议族 IPV4
    // server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 监听本地所有 IP 地址
    // server_addr.sin_port = htons(6666);              // 绑定端口号

    // addr.setSockAddr(server_addr);

    EchoServer server(&baseLoop,addr,"EchoServer");
    server.start();


    return 0;
}
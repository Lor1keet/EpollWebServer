#include "Webserver.h"

WebServer::WebServer()
{
    //http_conn类对象
    users = new http_conn[MAX_FD];

    //root文件夹路径
    char server_path[200];
    getcwd(server_path, 200);
    char root[6] = "/root";
    m_root = (char *)malloc(strlen(server_path) + strlen(root) + 1);
    strcpy(m_root, server_path);
    strcat(m_root, root);
}

WebServer::~WebServer()
{
    close(m_listenfd);
    close(m_pipefd[1]);
    close(m_pipefd[0]);
    delete[] users;
    delete m_pool;
}

void WebServer::trig_mode()
{
    //LT + LT
    if (0 == m_TRIGMode)
    {
        m_LISTENTrigmode = 0;
        m_CONNTrigmode = 0;
    }
    //LT + ET
    else if (1 == m_TRIGMode)
    {
        m_LISTENTrigmode = 0;
        m_CONNTrigmode = 1;
    }
    //ET + LT
    else if (2 == m_TRIGMode)
    {
        m_LISTENTrigmode = 1;
        m_CONNTrigmode = 0;
    }
    //ET + ET
    else if (3 == m_TRIGMode)
    {
        m_LISTENTrigmode = 1;
        m_CONNTrigmode = 1;
    }
}

void WebServer::init(int port, string user, string passWord, string databaseName, int log_write, 
                     int opt_linger, int trigmode, int sql_num, int thread_num, int close_log, int actor_model)
{
    m_port = port;
    m_user = user;
    m_passWord = passWord;
    m_databaseName = databaseName;
    m_sql_num = sql_num;
    m_thread_num = thread_num;
    m_log_write = log_write;
    m_OPT_LINGER = opt_linger;
    m_TRIGMode = trigmode;
    m_close_log = close_log;
    m_actormodel = actor_model;
}

void WebServer::thread_pool()
{
    //线程池
    m_pool = new threadpool<http_conn>(m_actormodel, m_connPool, m_thread_num);
}

void WebServer::eventListen()
{
    server_sock = new Socket(); 
    m_listenfd = server_sock->getFd();
    assert(m_listenfd >= 0);

    //优雅关闭连接
    if (0 == m_OPT_LINGER)
    {
        struct linger tmp = {0, 1};
        setsockopt(m_listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));
    }
    else if (1 == m_OPT_LINGER)
    {
        struct linger tmp = {1, 1};
        setsockopt(m_listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));
    }

    int ret = 0;
    InetAddress *address = new InetAddress(INADDR_ANY, m_port);

    int flag = 1;
    setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    server_sock->bind(address);
    server_sock->setnonblocking();
    server_sock->listen();

    epoll = new Epoll();

    epoll->addFd(m_listenfd, false, m_LISTENTrigmode);
    http_conn::m_epollfd = epoll;

}

bool WebServer::dealclientdata(){
    InetAddress client_addr;
    if (m_LISTENTrigmode == 0){
        int connfd = server_sock->accept(&client_addr);
        if (connfd < 0) {
            perror("LT mode accept error");
            return false;
        }
        users[connfd].init(connfd, client_addr.addr, m_root, m_CONNTrigmode, m_user, m_passWord, m_databaseName);
    }
    
    else {
        while(true){          
            int connfd = server_sock->accept(&client_addr);
            if (connfd < 0) {
                // 非阻塞模式下，无新连接时返回 EAGAIN，退出循环
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    break;
                }                
                perror("accept error");
                break;
            }
            users[connfd].init(connfd, client_addr.addr, m_root, m_CONNTrigmode, m_user, m_passWord, m_databaseName);
        }
    }
    return true;
}

void WebServer::dealwithread(int sockfd){
    if (m_actormodel == 1){
        m_pool->append(&users[sockfd], 0);
        while(true){
            if (users[sockfd].improv == 1){
                users[sockfd].improv = 0;
                break;
            }
        }
    }
    else {
        if (users[sockfd].read_once()){
            m_pool->append_p(&users[sockfd]);
        }
    }
}

void WebServer::eventLoop(){
    bool stop_server = false;
    
    while (!stop_server){
        int number = epoll_wait(epoll->getFd(), epoll->getEvents(), MAX_EVENT_NUMBER, -1);
        for (int i = 0; i < number; ++i){
            epoll_event event = epoll->getEvents()[i]; 
            int sockfd = event.data.fd;
            if (sockfd == m_listenfd){ // 有新客户端连接
                bool flag = dealclientdata();
                if (flag = false){
                    continue;
                }
            }
            else if (event.events & EPOLLIN){
                dealwithread(sockfd);
            }
            else if (event.events & EPOLLOUT){
                dealwithwrite(sockfd);
            }
        }
    }
}
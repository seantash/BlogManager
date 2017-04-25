

#include <BlogManHTTPClient.h>
#include <HTTPClientMsg.h>
#include <iostream>
#include <queue>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <semaphore.h>
#include <stdlib.h>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <fstream>

using namespace std;

pthread_t threadInfo;
const char* threadId = "BlogManHTTPClient";

void* httpClientThreadMain( void* );
BlogManHTTPClient* sysHttpClient = 0;

#define IP_STRING_LENGTH    16

BlogManHTTPClient::BlogManHTTPClient()
{
    m_prcsMsgSem = new sem_t;
    m_queLock = new pthread_mutex_t;
    m_msgQueue = 0;
    m_threadActive = true;
    m_socket = 0;
}

BlogManHTTPClient::~BlogManHTTPClient()
{
    delete m_prcsMsgSem;
    delete m_queLock;
    if(m_msgQueue !=0)
    {
        delete m_msgQueue;
    }
}

void BlogManHTTPClient::destroyClient()
{
    sysHttpClient->m_threadActive = false;
    sem_post(sysHttpClient->m_prcsMsgSem);
    pthread_join(threadInfo, NULL);
}

BlogManHTTPClient* BlogManHTTPClient::getClient()
{
    BlogManHTTPClient* retVal = 0;
    
    if(sysHttpClient == 0)
    {
        sysHttpClient = new BlogManHTTPClient();

        if (sem_init(sysHttpClient->m_prcsMsgSem, 0, 0) == -1)
        {
            cout << "BlogManHTTPClient::init Error: Unable to init semaphore." << endl;
            sysHttpClient->m_threadActive = false;
        }
        else
        {
            if (pthread_create(&threadInfo, NULL, httpClientThreadMain, (void *)threadId))
            {
                sysHttpClient->m_threadActive = false;
                cout << "BlogManHTTPClient::init Error: Unable to create thread." << endl;
            }
        }
        
        if(sysHttpClient->m_threadActive == true)
        {
            retVal = sysHttpClient;
        }
    }
    else
    {
        retVal = sysHttpClient;
    }
    
    return retVal;
}

bool BlogManHTTPClient::connectToServer(char* host)
{
    bool retVal = true;
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    if(m_socket < 0)
    {
        cout<<"BlogManHTTPClient::connectToServer Error: Creating socket failed!" << endl;
        retVal = false;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_aton(getIP(host), &addr.sin_addr);

    if(connect(m_socket, (sockaddr*)&addr, sizeof(addr)) != 0)
    {
        cout<<"BlogManHTTPClient::connectToServer Error: Connecting to server failed!"<< endl;
        retVal = false;
    }
    
    return retVal;
}

void BlogManHTTPClient::queueMsg(HTTPClientMsg* msg)
{
    pthread_mutex_lock(m_queLock);

    // Queue Msg
    m_msgQueue = msg;
    sem_post(m_prcsMsgSem);

    pthread_mutex_unlock(m_queLock);

    sem_wait(msg->m_reqDoneSem);
}

HTTPClientMsg* BlogManHTTPClient::dequeueMsg()
{
    HTTPClientMsg* msg = 0;
    
    pthread_mutex_lock(m_queLock);
    
    // Dequeue Msg
    msg = m_msgQueue;
    m_msgQueue = 0;
    
    pthread_mutex_unlock(m_queLock);
    
    return msg;
}

char* BlogManHTTPClient::buildGetPostsMsg(const char *host, const char *page)
{
    char *httpMsg;
    const char *msgOutline = "GET /%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
    
    //need to subtract by 5 for all the %s plus string terminator
    int msgLen = (strlen(host) + strlen(page) + strlen("HTMLGET 1.1") + strlen(msgOutline)-5);
    
    httpMsg = (char *)new char[msgLen];
    sprintf(httpMsg, msgOutline, page, host, "HTMLGET 1.1");
    return httpMsg;
}

char* BlogManHTTPClient::getIP(char *host)
{
    struct hostent *hent;
    char *ip = (char *)new char[IP_STRING_LENGTH];
    memset(ip, 0, IP_STRING_LENGTH);
    
    if((hent = gethostbyname(host)) == NULL)
    {
       cout<<"Couldn't find IP."<<endl;
    }
    
    if(inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ip, IP_STRING_LENGTH) == NULL)
    {
       cout<<"Can't resolve host."<<endl;
    }
    
    return ip;
}

void BlogManHTTPClient::processQueue()
{
    unsigned int msgType = HTTPClientMsg::HTTP_INVALID;

    while( m_threadActive )
    {
        sem_wait(m_prcsMsgSem);

        HTTPClientMsg* msg = dequeueMsg();

        if(msg != 0)
        {
            msgType = msg->m_type;
              
            if(msgType == HTTPClientMsg::HTTP_GET)
            { 
                if(connectToServer(msg->m_host))
                {
                    ofstream rspMsgfile;

                    char *getMsg = buildGetPostsMsg(msg->m_host, msg->m_page);
                    char response[8192];
                    send(m_socket, getMsg, strlen(getMsg), 0);
                    rspMsgfile.open (msg->m_outFilePath);

                    if (rspMsgfile.is_open()) 
                    { 
                        // We know the size so one time read from socket is good enough
                        if(recv(m_socket, response, 8192, 0 ) > 0)
                        {
                            rspMsgfile<<response;
                        }
                        else
                        {
                            cout<<"BlogManHTTPClient::processQueue Socket read Error!" << endl;
                        }
                    }
                    else
                    {
                        cout<<"BlogManHTTPClient::processQueue Error: oprning file to save server response failed!" << endl;
                    }
                    
                    rspMsgfile.close ();
                    sem_post(msg->m_reqDoneSem);
                }
            }
            else if (msgType == HTTPClientMsg::HTTP_POST)
            {
                cout<<"BlogManHTTPClient::processQueue Error: Post Message are not supported!" << endl;
            }
            else
            {
                cout<<"BlogManHTTPClient::processQueue Error: Received unknown message!" << endl;
            }
            
            delete msg;
        }
    }
}    
void* httpClientThreadMain(void *threadid)
{
    cout<<"BlogManHTTPClient Thread Sarted!" <<endl;

    if (sysHttpClient != 0)
    {
        sysHttpClient->processQueue();
    }
    
    pthread_exit(NULL);
    delete sysHttpClient;
    sysHttpClient = 0;
}

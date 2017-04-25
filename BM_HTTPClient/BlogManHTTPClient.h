
#ifndef BlogManHTTPClient_H
#define BlogManHTTPClient_H

#include <semaphore.h>
#include <HTTPClientMsg.h>
#include <pthread.h>

class BlogManHTTPClient 
{
    private:
        int m_socket;

        pthread_mutex_t* m_queLock;
        HTTPClientMsg* m_msgQueue;  
        
        bool connectToServer(char* host);
        HTTPClientMsg* dequeueMsg( void );
        char* buildGetPostsMsg(const char *host, const char *page);
        char* getIP(char *host);

        BlogManHTTPClient( void ); 
        
    public:
        bool m_threadActive;
        sem_t* m_prcsMsgSem;

        static BlogManHTTPClient* getClient( void );
        static void destroyClient( void );
        void processQueue( void );
        void queueMsg( HTTPClientMsg* msg);
        virtual ~BlogManHTTPClient( void );
};

#endif /* BlogManHTTPClient_H */


#ifndef HTTPClientMsg_H
#define HTTPClientMsg_H

#include <semaphore.h>

class HTTPClientMsg 
{
    public:
        static const unsigned int HTTP_INVALID;
        static const unsigned int HTTP_GET;
        static const unsigned int HTTP_POST;
    
    private:
        friend class BlogManHTTPClient;
        
        sem_t* m_reqDoneSem;
        char* m_outFilePath;
        char* m_host;
        char* m_page;
        int m_type;
        HTTPClientMsg* m_nextMsg;

    public:
        HTTPClientMsg( int type, sem_t* reqDoneSem, const char* host, const char* page, const char* outFilePath); 
        virtual ~HTTPClientMsg();
};

#endif /* HTTPClientMsg_H */

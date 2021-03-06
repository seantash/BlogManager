
#include <HTTPClientMsg.h>
#include <string.h>

using namespace std;

const unsigned int HTTPClientMsg::HTTP_INVALID = 0xFF;
const unsigned int HTTPClientMsg::HTTP_GET = 0xFE;
const unsigned int HTTPClientMsg::HTTP_POST = 0xFD;


HTTPClientMsg::HTTPClientMsg( int type, sem_t* reqDoneSem, const char* host, const char* page, const char* outFilePath) 
{
    int hostLen = strlen(host);
    m_host = new char[hostLen];    
    memcpy(m_host, host, hostLen);
    
    int pageLen = strlen(page);
    m_page = new char[pageLen];
    memcpy(m_page, page, pageLen);

    int filePathLen = strlen(host);
    m_outFilePath = new char[filePathLen]; 
    memcpy(m_outFilePath, outFilePath, filePathLen);
    m_type = type;
    m_reqDoneSem = reqDoneSem;
    m_nextMsg = 0;
}

HTTPClientMsg::~HTTPClientMsg()
{
    delete m_host;
    delete m_page;
    delete m_outFilePath;
}

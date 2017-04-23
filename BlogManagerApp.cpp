
// This App retrieves and displays specific posts using  libCURL to get the actual JSON data. 
// Then parse it with some C++ library like jsoncpp.

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fstream>
#include <iostream>

//thread stuff
#include <pthread.h>
#include <semaphore.h>  /* required for semaphores */
#include <cstdlib>
#include <errno.h>

using namespace std;
#define IP_STRING_LENGTH        16
#define HTTP_RESPONSE_MSG_FILE  "postsFromServer"
static sem_t rspSem;


class PostInfo 
{
    private:
        friend class PostList;

        int m_userId;
        int m_id;
        char* m_title;
        char* m_body;
        PostInfo* m_nextPost;

    public:
        PostInfo ( int userId, int id, char* title, char* body); // Ownership of passed in memory is not assumed.
        virtual ~PostInfo ( void );
        void print();
};


PostInfo::PostInfo ( int userId, int id, char* title, char* body): m_userId(userId), m_id(id), m_nextPost(0), m_title(title), m_body(body)
{
    //int numChar = strlen(title)+1;
    //m_title = new char[numChar];
    //memcpy(m_title, title, numChar);
    //
    //numChar = strlen(body)+1;
    //m_body = new char[numChar];
    //memcpy(m_body, body, numChar);
}

PostInfo::~PostInfo ( void )
{
    delete m_title;
    delete m_body;
}

void PostInfo::print ( void )
{
    cout<<"[POST-ENTRY]"<<endl;
    cout<<"  User ID: "<<m_userId<<endl;
    cout<<"  Post ID: "<<m_id<<endl;
    cout<<"    TITLE: "<<m_title<<endl;
    cout<<"    BODY:: "<<m_body<<endl;
    cout<<"[POST-ENTRY]"<<endl<<endl;
}

class PostList 
{
    private:
        PostInfo* m_head;
        PostInfo* m_tail;
        int m_size;
        
    public:
        PostList ( );
        virtual ~PostList ();
        void addPost( PostInfo* post ); // Ownership of passed in memeory is assumed
        void print( void );
};

PostList::PostList ( ) : m_head(0), m_tail(0), m_size(0)
{

}

PostList::~PostList ( void )
{
    PostInfo* deletePost = m_head;
    
    while(deletePost != 0)
    {
        PostInfo* nextToDel = deletePost->m_nextPost;
        delete deletePost;
        deletePost = nextToDel;
    }
}

void PostList::print ( void )
{
    PostInfo* printPost = m_head;
    
    if( m_size == 0 )
    {
        cout<<"There are No Posts."<<endl;
    }
    else
    {
        cout<<"There are " << m_size << " Posts."<<endl;
    }
    
    while(printPost != 0)
    {
        PostInfo* nextToPrint = printPost->m_nextPost;
        printPost->print();
        printPost = nextToPrint;
    }
}

void PostList::addPost( PostInfo* post )
{
    if(m_head == 0)
    {
        m_head = post;
        if(m_tail != 0)
        {
            delete m_tail;
        }
        m_tail = post;
        m_size++;
    }
    
    else if( (m_head != 0) && (m_tail != 0) && (m_tail == m_head) )
    {
        m_head->m_nextPost = post;
        m_tail = post;
        m_size++;
    }

    else if( (m_head != 0) && (m_tail != 0) && (m_tail != m_head) )
    {
        m_tail->m_nextPost = post;
        m_tail = post;
        m_size++;
    }
}

char* getSubString( const char* s1, const char* s2, int rmCharFront, int  rmCharBack)
{
    int sLen1 = strlen(s1);
    int sLen2 = strlen(s2);
    char* retVal = 0;
    int i = 0, j = 0;
        
    while ( (i<sLen1) && ((sLen1 - i) >= sLen2))
    {
        while (s1[i] == s2[j])
        {
            i++;
            j++;
            if(j == sLen2)
            {
                int restOfStrLen = (sLen1 - i) -rmCharFront - rmCharBack;
                retVal = new char [restOfStrLen];
                memcpy(retVal, s1+(i+rmCharFront), restOfStrLen);
                return (retVal);
            }
        }
        j=0;
        i++;
    }
    
    return retVal;
}

char* buildGetPostsMsg(const char *host, const char *page)
{
    char *httpMsg;
    char *msgOutline = "GET /%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";

    //need to subtract by 5 for all the %s plus string terminator
    int msgLen = (strlen(host)+strlen(page)+strlen("HTMLGET 1.1")+strlen(msgOutline)-5);
    
    httpMsg = (char *)new char[msgLen];
    sprintf(httpMsg, msgOutline, page, host, "HTMLGET 1.1");
    return httpMsg;
}

char*  getIP(char *host)
{
    struct hostent *hent;
    char *ip = (char *)new char[IP_STRING_LENGTH];
    memset(ip, 0, IP_STRING_LENGTH);
    
    if((hent = gethostbyname(host)) == NULL){
       printf("Couldn't find IP");
       exit(1);
    }
    
    if(inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ip, IP_STRING_LENGTH) == NULL){
       cout<<"Can't resolve host"<<endl;
       exit(1);
    }
    
    return ip;
}

int createTCPSocketForHTTPClient()
{
    int s, error;
    struct sockaddr_in addr;
    ofstream rspMsgfile;

    if((s = socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        cout<<"Error 01: creating socket failed!\n";
        return 1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_aton(getIP("demo7701733.mockable.io"),&addr.sin_addr);

    error = connect(s,(sockaddr*)&addr,sizeof(addr));
    if(error!=0)
    {
        cout<<"Error 02: conecting to server failed!\n";
        return 1;
    }

    char *getMsg = buildGetPostsMsg("demo7701733.mockable.io", "posts");

    char response[8192];
    send(s, getMsg, strlen(getMsg),0);

    rspMsgfile.open (HTTP_RESPONSE_MSG_FILE);
            
    if (rspMsgfile.is_open()) { 
       
        // We know the size so one time read from socket is good enough
        if(recv(s, response, 8192, 0 ) > 0)
        {
            rspMsgfile<<response;
        }
        else
        {
            cout<<"Socket read Error!\n";
        }
    }
    else
    {
        cout<<"Error 03: oprning file to save server response failed!\n";
        return 1;
    }
    
    rspMsgfile.close ();
    sem_post(&rspSem);         /* end of critical section */
    
    return error;
}

void *httpClientMain(void *threadid) {
    long tid;
    tid = (long)threadid;
    
    createTCPSocketForHTTPClient();
    
    pthread_exit(NULL);
}

void processResponse() {
    ifstream postsFile;

    std::cout << "Processing The Posts From Blog Server\n";
    
    postsFile.open (HTTP_RESPONSE_MSG_FILE);
    string line;

    if (postsFile.is_open()) 
    { 
        // Walk the message verify http fields
        if( (getline(postsFile,line)) && (memcmp(line.c_str(), "HTTP/1.1 200 OK", strlen("HTTP/1.1 200 OK")) != 0) )
        {
            cout << "HTTP Response is Invalid." << endl;
        }
        else if( (getline(postsFile,line)) && (memcmp(line.c_str(), "access-control-allow-origin", strlen("access-control-allow-origin")) != 0) )
        {
            cout << "HTTP access control missing." << endl;
        }
        else if( (getline(postsFile,line)) && (memcmp(line.c_str(), "Content-Type: application/json", strlen("Content-Type: application/json")) != 0) )
        {
            cout << "REST Message Format is not JSON as Expected." << endl;
        }
        else if( (getline(postsFile,line)) && (memcmp(line.c_str(), "X-Cloud-Trace-Context", strlen("X-Cloud-Trace-Context")) != 0) )
        {
            cout << "X-Cloud-Trace-Context was not reported." << endl;
        }
        else if( (getline(postsFile,line)) && (memcmp(line.c_str(), "Date", strlen("Date")) != 0) )
        {
            cout << "Date was not reported." << endl;
        }
        else if( (getline(postsFile,line)) && (memcmp(line.c_str(), "Server", strlen("Server")) != 0) )
        {
            cout << "Host name was not reported." << endl;
        }
        else
        {
            // Now do the http body and size
            if( (getline(postsFile,line)) && (memcmp(line.c_str(), "Content-Length: ", strlen("Content-Length: ")) == 0) )
            {
                 int numChar = strlen(line.c_str()) - strlen("Content-Length: ");
                 char *msgContLen = new char[numChar];
                 memcpy(msgContLen, line.c_str() + strlen("Content-Length: "), numChar);
                 int msgBodyLen = atoi(msgContLen);
                 delete msgContLen;
                 
                 PostList* postsFromBlog = new PostList( );

                 while ( getline (postsFile,line) )
                 {
                     char* value = getSubString(line.c_str(), "userId", 2, 1);
                     if(value != 0)
                     {
                         // build the post object
                         int userID = atoi(value);
                         int postID = 0;
                         char* pTitle = 0;
                         char* pBody = 0;
                         
                         if( getline(postsFile,line) )
                         {
                             value = getSubString(line.c_str(), "id", 2, 1);
                             if(value != 0)
                             {
                                 postID = atoi(value);
                                 
                                 if( getline(postsFile,line) )
                                 {
                                     value = getSubString(line.c_str(), "title", 4, 2);
                                     if(value != 0)
                                     {
                                         pTitle = value;
                                         
                                         if( getline(postsFile,line) )
                                        {
                                            value = getSubString(line.c_str(), "body", 4, 1);
                                            if(value != 0)
                                            {
                                                pBody = value;
                                            } 
                                        }
                                     }
                                 }
                             }
                         }
                         postsFromBlog->addPost(new PostInfo(userID, postID, pTitle, pBody));
                     }
                 }
                 
                 cout << '\n' << endl;
                 postsFromBlog->print();
            }
            else
            {
                cout << "Missing Content Length Info." << endl;
            }
        }
        postsFile.close();
    }
    else
    {
        cout<<"Reading the posts failed!\n";
    }
}

int main ()
{

    std::cout << "\n\nWelcome To Blog Manager!\n\n";
    
    if (sem_init(&rspSem, 0, 0) == -1)
    { 
        printf("sem_init: failed: %s\n", strerror(errno)); 
    }
    
    pthread_t clientThread;
    int threadResult;
    int cThreadId=99;
    
    threadResult = pthread_create(&clientThread, NULL, httpClientMain, (void *)cThreadId);
    
    if (threadResult){
        cout << "Error:unable to create thread," << threadResult << endl;
        exit(-1);
    }

    sem_wait(&rspSem);         /* start of critical section */
    processResponse();

    pthread_exit(NULL);
    
    return 0;
}




/*

favorite
3
I need to write a command line client for playing tic-tac-toe over a server. the server accepts http requests and sends back json to my client. i am looking for a quick way to send a http request and receive the json as a string using boost libraries.

example http request = "http://???/newGame?name=david"
example json response = "\"status\":\"okay\", \"id\":\"game-23\", \"letter\":2"
c++ json http boost
shareimprove this question
asked Nov 5 '14 at 15:37

Mladen Kajic
4717
  	 	
Note for serious business (mainly existing webservers that may do any manner of chunked encoding, compressions, keep-alive, redirect responses etc.) you will want to use a library like curl.haxx.se/libcurl – sehe Nov 5 '14 at 16:33 
add a comment
1 Answer
active oldest votes
up vote
5
down vote
accepted
The simplest thing that fits the description:

Live On Coliru

#include <boost/asio.hpp>
#include <iostream>

int main() {
    boost::system::error_code ec;
    using namespace boost::asio;

    // what we need
    io_service svc;
    ip::tcp::socket sock(svc);
    sock.connect({ {}, 8087 }); // http://localhost:8087 for testing

    // send request
    std::string request("GET /newGame?name=david HTTP/1.1\r\n\r\n");
    sock.send(buffer(request));

    // read response
    std::string response;

    do {
        char buf[1024];
        size_t bytes_transferred = sock.receive(buffer(buf), {}, ec);
        if (!ec) response.append(buf, buf + bytes_transferred);
    } while (!ec);

    // print and exit
    std::cout << "Response received: '" << response << "'\n";
}
This receives the full response. You can test it with a dummy server:
(also Live On Coliru):

netcat -l localhost 8087 <<< '"status":"okay", "id":"game-23", "letter":2'
*/
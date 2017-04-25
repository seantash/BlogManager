
// This App retrieves and displays specific posts using  libCURL to get the actual JSON data. 
// Then parse it with some C++ library like jsoncpp.

#include <HTTPClientMsg.h>
#include <PostServices.h>
#include <BlogManHTTPClient.h>
#include <semaphore.h>
#include <iostream>

#define POSTS_HTTP_GET_RSP_FILE  "./getPostsRspFromServer"

using namespace std;

int main()
{
    std::cout << "\n\nWelcome To Blog Manager!\n\n";
    
    BlogManHTTPClient* httpClient = 0;
    HTTPClientMsg* httpMsg = 0;
    PostList* pList = 0;
    PostServices* blogManager = new PostServices();

    sem_t* httpReqCompleteSem = new sem_t;

    if (sem_init(httpReqCompleteSem, 0, 0) == -1)
    {
        cout <<"main() - sem_init failed!"<< endl;
        return 0;
    }
    
    // Get the http client. The http client has a singleton design pattern so no multi instances.
    httpClient = BlogManHTTPClient::getClient();

    if(httpClient != 0)
    {
        // Request the blog posts via http client
        httpMsg = new  HTTPClientMsg( HTTPClientMsg::HTTP_GET, httpReqCompleteSem, "demo7701733.mockable.io", "posts", POSTS_HTTP_GET_RSP_FILE);
        httpClient->queueMsg( httpMsg );
    }

    std::cout << "Processing The Posts From Blog Server\n";
    pList = blogManager->processHTTPResponse(POSTS_HTTP_GET_RSP_FILE);
    
    if(pList != 0)
    {
        pList->print();
    }
    
    BlogManHTTPClient::destroyClient();
    
    return 0;
}



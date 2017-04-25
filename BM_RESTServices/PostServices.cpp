
#include <PostInfo.h>
#include <PostList.h>
#include <PostServices.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>

using namespace std;


PostServices::PostServices()
{
    
} 

PostServices::~PostServices( void )
{
    
}

char* PostServices::getJsonValueStrForNameStr( const char* s1, const char* s2, int rmCharFront, int  rmCharBack)
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

PostList* PostServices::processHTTPResponse(const char* filePath)
{
    PostList* postsFromBlog = new PostList( );
    ifstream postsFile;

    postsFile.open( filePath );
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
                 
                 if(msgBodyLen > 0)
                 {
                     while ( getline (postsFile,line) )
                     {
                         char* value = getJsonValueStrForNameStr(line.c_str(), "userId", 2, 1);
                         if(value != 0)
                         {
                             // build the post object
                             int userID = atoi(value);
                             int postID = 0;
                             char* pTitle = 0;
                             char* pBody = 0;
                             
                             if( getline(postsFile,line) )
                             {
                                 value = getJsonValueStrForNameStr(line.c_str(), "id", 2, 1);
                                 if(value != 0)
                                 {
                                     postID = atoi(value);
                                     
                                     if( getline(postsFile,line) )
                                     {
                                         value = getJsonValueStrForNameStr(line.c_str(), "title", 4, 2);
                                         if(value != 0)
                                         {
                                             pTitle = value;
                                             
                                             if( getline(postsFile,line) )
                                            {
                                                value = getJsonValueStrForNameStr(line.c_str(), "body", 4, 1);
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
                 }
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
    
    return postsFromBlog;
}




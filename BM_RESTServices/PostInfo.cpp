
#include <PostInfo.h>
#include <iostream>

using namespace std;


PostInfo::PostInfo ( int userId, int id, char* title, char* body)
{
    m_userId = userId; 
    m_id = id;
    m_nextPost = 0;
    m_title = title; 
    m_body = body;
}

PostInfo::~PostInfo ( void )
{
    delete m_title;
    delete m_body;
}

void PostInfo::print ( void )
{
    cout<<"<POST-START>"<<endl;
    cout<<"  User ID: "<<m_userId<<endl;
    cout<<"  Post ID: "<<m_id<<endl;
    cout<<"    TITLE: "<<m_title<<endl;
    cout<<"    BODY:: "<<m_body<<endl;
    cout<<"<POST-END>"<<endl<<endl;
}

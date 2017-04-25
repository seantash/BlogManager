
#include <PostInfo.h>
#include <PostList.h>
#include <iostream>

using namespace std;


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
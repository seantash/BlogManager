
#ifndef PostInfo_H
#define PostInfo_H

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

#endif /* PostInfo_H */

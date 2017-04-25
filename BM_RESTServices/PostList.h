
#ifndef PostList_H
#define PostList_H

class PostInfo;

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

#endif /* PostList_H */

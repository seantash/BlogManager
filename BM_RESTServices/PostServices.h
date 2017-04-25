
#ifndef PostService_H
#define PostService_H

#include <PostList.h>

class PostServices 
{
    private:
        char* getJsonValueStrForNameStr( const char* s1, const char* s2, int rmCharFront, int rmCharBack);

    public:
        PostServices ( void ); 
        virtual ~PostServices ( void );
        PostList* processHTTPResponse(const char* filePath);
};

#endif /* PostService_H */


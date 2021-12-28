#include <stdio.h>
#include <stdlib.h>
#include <string.h>

template<class A, class B>
class LibMap
{
    private:
        int counter = 0;
    public:
        A Key[100];
        B Value[100];

        void insert(A k, B val);
        void clear();
 
        B operator [] (A t);
};

template<class A, class B>
void LibMap<A,B>::clear()
{
    memset(Key, 0, sizeof(Key));
    memset(Value, 0, sizeof(Value));
    counter = 0;
}

template<class A, class B>
void LibMap<A,B>::insert(A k, B val)
{
      Key[counter] = k;
    Value[counter] = val;
    ++counter;
}

template<class A, class B>
B LibMap<A, B>::operator[](A t) 
{
    B tmp;
    for(int index = 0;index<=counter;index++) {
        if(t == Key[index]) {
            return Value[index];
        }
    } 
    return tmp;
}

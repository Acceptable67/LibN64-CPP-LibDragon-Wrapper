#include <stdio.h>
#include <stdlib.h>

template<class T>
class LibVector 
{
	public:
		T *array;
		int counter = 0;
		
		LibVector(int size = 0x1000);

		void push_back(T item);
		void insert   (int index, T item);
		void replace  (int index, T item);
		int  length   ();
		T    at       (int index);
};
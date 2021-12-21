#include <stdio.h>
#include <stdlib.h>

template<class T>
class LibVector {
	public:
		T *array;
		
		int counter = 0;
		
		LibVector(int size = 0x1000)
		{
			array = (T*)malloc(size);
		}

		void push_back(T item)
		{
			array[counter] = item;
			counter++;
		}

		void insert(int index, T item)
		{
    	 	int size = length() + 1;
			for (int i = size-1; i >= index; i--)
				array[i] = array[i - 1];

			array[index - 1] = item;
			++counter;
		}
		
		void replace(int index, T item)
		{
			array[index] = item;
		}
		
		int length() { 
			return counter; 
		}
		
		T at(int index)
		{
			return array[index];
		}
};
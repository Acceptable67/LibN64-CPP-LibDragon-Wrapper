#include "LibN64.h"
#include "LibVector.h"
#include "LibString.h"
template<class T>
LibVector<T>::LibVector(int size)
{
	this->array = (T*)malloc(size);
}

template<class T>
void LibVector<T>::push_back(T item)
{
	array[counter] = item;
	counter++;
}

template<class T>
void LibVector<T>::insert(int index, T item)
{
    int size = length() + 1;
	for (int i = size-1; i >= index; i--)
		array[i] = array[i - 1];

	array[index - 1] = item;
#include <stdio.h>
	++counter;
}
		
template<class T>
void LibVector<T>::replace(int index, T item)
{
	array[index] = item;
}

template<class T>
int LibVector<T>::length() { 
	return counter; 
}

template<class T>	
T LibVector<T>::at(int index)
{
	return array[index];
}


template class LibVector<unsigned int>;
template class LibVector<unsigned long>;
template class LibVector<unsigned short>;
template class LibVector<unsigned char>;
template class LibVector<unsigned char*>;
template class LibVector<bool>;
template class LibVector<LibN64::Frame::LibPos>;
template class LibVector<LibString>;
template class LibVector<int>;
template class LibVector<long>;
template class LibVector<double>;
template class LibVector<short>;
template class LibVector<char>;
template class LibVector<char*>;
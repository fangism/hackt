// "art_utils.tcc"
// template methods from art_utils.h
// include this to use them

#include <iostream>
// #include <fstream>

#include "art_utils.h"


template <class T>
inline
void    write_value(ostream& f, const T& v) {
	f.write((const char*) &v, sizeof(T));
}

template <class T>
inline
void    read_value(istream& f, T& v) {
	f.read((char*) &v, sizeof(T));
}



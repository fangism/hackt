/**
	\file "art_utils.tcc"
	Template function definitions from "art_utils.h".
 */

#ifndef __ART_UTILS_TCC__
#define __ART_UTILS_TCC__

#include <iostream>

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

#endif	// __ART_UTILS_TCC__


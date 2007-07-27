// \file "gcdstatic.cc"
// A plain static library.  
//	$Id: gcdstatic.cc,v 1.1.2.1 2007/07/27 02:39:30 fang Exp $

#include "gcdstatic.h"

/**
	\pre a >= b
 */
static
int
__gcd(const int a, const int b) {
	if (b == 0)
		return a;
	else if (b == 1)
		return 1;
	else	return __gcd(b, a%b);
}

int
gcd(const int a, const int b) {
	if (a >= b)
		return __gcd(a, b);
	else	return __gcd(b, a);
}


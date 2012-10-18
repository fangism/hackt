// \file "gcdstatic.cc"
// A plain static library.  
//	$Id: gcdstatic.cc,v 1.2 2007/07/31 23:24:20 fang Exp $

#include "gcdstatic.hh"

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


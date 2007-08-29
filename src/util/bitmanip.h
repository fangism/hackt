/**
	\file "util/bitmanip.h"
	For unifying the interface to built-in bit manipulation functions.
	Name-overloaded wrappers around compiler built-in functions.  
	$Id: bitmanip.h,v 1.1 2007/08/29 04:45:48 fang Exp $
 */

#ifndef	__UTIL_BITMANIP_H__
#define	__UTIL_BITMANIP_H__

#include "util/numeric/popcount.h"
#include "util/numeric/parity.h"
#include "util/numeric/clz.h"
#include "util/numeric/ctz.h"
#include "util/numeric/ffs.h"

namespace util {
namespace numeric {
using numeric::popcount;
using numeric::parity;
using numeric::clz;
using numeric::ctz;
using numeric::ffs;
using numeric::msb;
using numeric::lsb;

}	// end namespace numeric
}	// end namespace util

#endif	// __UTIL_BITMANIP_H__


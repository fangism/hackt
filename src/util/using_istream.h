/**
	\file "using_istream.h"
	Convenient using directives for istream, cin...
	Note: this is not wrapped by include-once protection, because it may
	be used within other namespaces.  
	Also assumes that <iosfwd> was included beforehand.  
	$Id: using_istream.h,v 1.1 2004/12/16 01:08:53 fang Exp $
 */

#ifndef	__USING_ISTREAM_H__
#define	__USING_ISTREAM_H__

using std::istream;
using std::cin;
// any others?

#endif	// __USING_ISTREAM_H__
#undef	__USING_ISTREAM_H__



/**
	\file "using_ostream.h"
	Convenient using directives for ostream, cout, cerr, endl...
	Note: this is not wrapped by include-once protection, because it may
	be used within other namespaces.  
	Also assumes that <iosfwd> was included beforehand.  
	$Id: using_ostream.h,v 1.1 2004/12/05 05:07:26 fang Exp $
 */

#ifndef	__USING_OSTREAM_H__
#define	__USING_OSTREAM_H__

using std::ostream;
using std::cout;
using std::cerr;
using std::endl;
// any others?

#endif	// __USING_OSTREAM_H__
#undef	__USING_OSTREAM_H__



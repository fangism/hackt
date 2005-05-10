/**
	\file "util/using_ostream.h"
	Convenient using directives for ostream, cout, cerr, endl...
	Note: this is not wrapped by include-once protection, because it may
	be used within other namespaces.  
	Also assumes that <iosfwd> was included beforehand.  
	$Id: using_ostream.h,v 1.2 2005/05/10 04:51:31 fang Exp $
 */

using std::ostream;
using std::cout;
using std::cerr;
using std::endl;
// any others?


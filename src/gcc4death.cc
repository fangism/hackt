/**
	\file "gcc4death.cc"
	ICE with gcc-4.0 prerelease snapshot 20050130.
	caused by:
	gcc-4.0 -c gcc4death.cc -o gcc4death.o

	feedback:
util/persistent_object_manager_gcc4death.h: In instantiation of 'util::persistent_traits<whatever>':
gcc4death.cc:7:   instantiated from here
util/persistent_object_manager_gcc4death.h:480: error: use of 'persistent_object_manager' is ambiguous
util/persistent_object_manager_gcc4death.h:67: error:   first declared as 'class util::persistent_object_manager' here
util/persistent_object_manager_gcc4death.h:28: error:   also declared as 'struct util::memory::persistent_object_manager' here
util/persistent_object_manager_gcc4death.h:480: error: conflicting declaration 'struct util::persistent_object_manager'
util/persistent_object_manager_gcc4death.h:67: error: 'class util::persistent_object_manager' has a previous declaration as 'class util::persistent_object_manager'
util/persistent_object_manager_gcc4death.h:480: internal compiler error: tree check: expected class 'declaration', have 'exceptional' (error_mark) in pushtag, at cp/name-lookup.c:4658
Please submit a full bug report,
with preprocessed source if appropriate.
See <URL:http://gcc.gnu.org/bugs.html> for instructions.

	Submitted to gcc bugzilla database as bug report #19948.

	$Id: gcc4death.cc,v 1.1.2.1 2005/02/14 07:56:30 fang Exp $
 */

#include "util/persistent_object_manager_gcc4death.h"

class whatever { };

static const
util::persistent_traits<whatever> __blah__;
	// is uninitialized, yes





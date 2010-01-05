/**
	A reduced test case showing a failure only on 10.6, 
	with all compilers apple-gcc-4.0, 4.2, and FSF's gcc-4.4.
	Linker issue was discovered when Peter O'Gorman (pogma)
	suggested adding LDFLAGS=-Wl,-no_compact_linkedit
	after which the test started passing.
 */
#define	ENABLE_STACKTRACE		1

#include <iostream>
#include "Object/common/namespace.h"
#include "Object/traits/bool_traits.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/inst/instance_placeholder_base.h"
#include "Object/inst/datatype_instance_placeholder.h" // DEBUG ONLY
#include "Object/inst/instance_placeholder.h" // DEBUG ONLY
#include "Object/ref/meta_reference_union.h"
// #include "util/memory/count_ptr.tcc"
#include "util/stacktrace.h"
// #include "ctest.h"

using namespace std;
using namespace HAC::entity;
using namespace util;
using namespace util::memory;
typedef	simple_meta_instance_reference<bool_tag>	bool_ref_type;

#if 0
template <class Base, class Derived>
inline
static
void
cast_test(const count_ptr<const Base>& bp, ostream& o) {
	STACKTRACE_VERBOSE;
	o << "bp @ " << &*bp << endl;
	const Derived* rdp = dynamic_cast<const Derived*>(&*bp);
	o << "rdp @ " << rdp << endl;
	const count_ptr<const Derived>
		dp(bp.template is_a<const Derived>());
	o << "dp @ " << &*dp << endl;
	o << endl;
}

static
void
inst_ref_test(ostream& o) {
	STACKTRACE_VERBOSE;
	const name_space ss("blank");
	const bool_ref_type::instance_placeholder_type ph(ss, "fake", 0);
	const bool_ref_type::instance_placeholder_ptr_type php(&ph);
	const count_ptr<const meta_instance_reference_base>
		dp(new bool_ref_type(php));
	const count_ptr<const bool_ref_type>
		ddp(new bool_ref_type(php));
	dp->what(o << "main:dp->what(): ") << endl;
	ddp->what(o << "main:ddp->what(): ") << endl;
	cast_test<meta_instance_reference_base, bool_ref_type>(dp, o);
	cast_test<meta_instance_reference_base, bool_ref_type>(ddp, o);
	o << endl;
}
#endif

static
void
expanded_test(ostream& o) {
	STACKTRACE_VERBOSE;
// bug is sufficiently demonstrated with bare pointers
#define	USE_COUNT_PTR		0
// de-const-ifying makes no difference
#define CONST	const
	const name_space ss("blank");
	const bool_ref_type::instance_placeholder_type ph(ss, "fake", 0);
	const bool_ref_type::instance_placeholder_ptr_type php(&ph);
#if USE_COUNT_PTR
	const count_ptr<CONST meta_instance_reference_base>
#else
	CONST meta_instance_reference_base* const
#endif
		dp(new bool_ref_type(php));
	o << "dp @ " << &*dp << endl;
	dp->what(o << "dp->what(): ") << endl;
	CONST bool_ref_type* const rcdp = IS_A(CONST bool_ref_type*, &*dp);
	o << "rcdp = " << rcdp << endl;
#if USE_COUNT_PTR
	const count_ptr<CONST bool_ref_type>
		cdp(dp.is_a<CONST bool_ref_type>());
	o << "cdp = " << &*cdp << endl;
#endif
	if (rcdp) {
		rcdp->what(o << "cdp->what(): ") << endl;
	}
#if USE_COUNT_PTR
	if (cdp) {
		cdp->what(o << "cdp->what(): ") << endl;
	}
#endif
	o << endl;
	INVARIANT(rcdp);
#if USE_COUNT_PTR
	INVARIANT(cdp);
#endif
#undef	CONST
#undef	USE_COUNT_PTR
}

#if 0
template <class B, class D>
static
void simple_test(ostream& o) {
	STACKTRACE_VERBOSE;
	const count_ptr<const B> bp(new D());
	const count_ptr<const D> dp(new D());
	cast_test<B, D>(bp, o);
	cast_test<B, D>(dp, o);
}
#endif

int
main(int argc, char* argv[]) {
	expanded_test(cout);
#if 0
if (0) {
	inst_ref_test(cout);
	simple_test<base, derived>(cout);
}
#endif
	return 0;
}


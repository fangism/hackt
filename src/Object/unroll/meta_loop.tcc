/**
	\file "Object/unroll/meta_loop.tcc"
	$Id: meta_loop.tcc,v 1.6 2011/02/08 22:32:52 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_META_LOOP_TCC__
#define	__HAC_OBJECT_UNROLL_META_LOOP_TCC__

#include <iostream>
#include <vector>

#include "Object/unroll/meta_loop.hh"
#include "Object/unroll/unroll_context.hh"
#include "Object/common/dump_flags.hh"
#include "Object/def/footprint.hh"
#include "Object/expr/const_range.hh"
#include "Object/expr/expr_dump_context.hh"
#include "Object/inst/pint_value_collection.hh"
#include "Object/inst/value_placeholder.hh"
#include "Object/inst/value_scalar.hh"
#include "Object/traits/pint_traits.hh"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/persistent_functor.tcc"
#include "util/indent.hh"
#include "util/stacktrace.hh"
#include "util/IO_utils.hh"


namespace HAC {
namespace entity {
using util::auto_indent;
#include "util/using_ostream.hh"
using PRS::rule_dump_context;
using util::read_value;
using util::write_value;

//=============================================================================
// class meta_loop<T> method definitions
// mostly ripped from "Object/lang/PRS.cc":rule_loop

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
template <class C>
ostream&
meta_loop<T>::dump(const T& mc, ostream& o, const C& c, const char op, 
	ostream& (T::implementation_type::*dumper)(ostream&, const C&) const) {
	NEVER_NULL(mc.ind_var);
	NEVER_NULL(mc.range);
	o << '(' << op << mc.ind_var->get_name() << ':';
	mc.range->dump(o, entity::expr_dump_context(c)) << ':' << endl;
	{
		INDENT_SECTION(o);
		(mc.*dumper)(o, c);
	}
	return o << auto_indent << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls a set of loop-dependent spec directives.  
	\param str diagnostic string.  
 */
template <class T>
good_bool
meta_loop<T>::unroll(const T& mc, const unroll_context& c, 
		const char* str) {
	// most of this copied from expr_loop_base::unroll...
	STACKTRACE_VERBOSE;
	// first, resolve bounds of the loop range, using current context
	const_range cr;
	if (!mc.range->unroll_resolve_range(c, cr).good) {
		cerr << "Error resolving range expression in " << str << ": ";
		mc.range->dump(cerr, entity::expr_dump_context::default_value)
			<< endl;
		return good_bool(false);
	}
	const pint_value_type min = cr.lower();
	const pint_value_type max = cr.upper();
	// if range is empty or backwards, then ignore
	if (min > max) {
		return good_bool(true);
	}
	// range gives us upper and lower bound of loop
	// in a loop:
	// create context chain of lookup
	//	using unroll_context's template_formal/actual mechanism.  
	// copied from loop_scope::unroll()
	DECLARE_TEMPORARY_FOOTPRINT(f);
	const never_ptr<pint_scalar>
		var(mc.initialize_footprint(f));
	// create a temporary by unrolling the placeholder 
	// induction variable into the footprint as an actual variable
	pint_value_type& p(var->get_instance().value);  
		// acquire direct reference
	const unroll_context cc(&f, c);
	for (p = min; p <= max; ++p) {
	if (!mc.implementation_type::unroll(cc).good) {
		cerr << "Error resolving " << str << " in loop:"
			<< endl;
		mc.ind_var->dump_qualified_name(cerr, dump_flags::verbose)
			<< " = " << p << endl;
		return good_bool(false);
	}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
void
meta_loop<T>::collect_transient_info(const T& mc, 
		persistent_object_manager& m) {
if (!m.register_transient_object(&mc, util::persistent_traits<T>::type_key)) {
	mc.meta_loop_base::collect_transient_info_base(m);
	mc.implementation_type::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
void
meta_loop<T>::write_object(const T& mc, const persistent_object_manager& m, 
		ostream& o) {
	mc.meta_loop_base::write_object_base(m, o);
	mc.implementation_type::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
void
meta_loop<T>::load_object(T& mc, const persistent_object_manager& m, 
		istream& i) {
	mc.meta_loop_base::load_object_base(m, i);
	mc.implementation_type::load_object_base(m, i);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC


#endif	// __HAC_OBJECT_UNROLL_META_LOOP_TCC__

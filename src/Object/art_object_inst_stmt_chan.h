/**
	\file "Object/art_object_inst_stmt_chan.h"
	Contains definition of nested, specialized class_traits types.  
	$Id: art_object_inst_stmt_chan.h,v 1.4.10.7 2005/07/15 03:49:06 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_STMT_CHAN_H__
#define	__OBJECT_ART_OBJECT_INST_STMT_CHAN_H__

#include <iostream>
#include "Object/traits/chan_traits.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_instance_chan.h"
#include "Object/inst/general_collection_type_manager.h"
#include "util/persistent_object_manager.h"
#include "Object/art_object_inst_stmt_type_ref_default.h"

namespace ART {
namespace entity {
class param_expr_list;
class const_param_expr_list;
#include "util/using_ostream.h"

//=============================================================================
#if 0
class class_traits<channel_tag>::instantiation_statement_type_ref_base {
public:
	typedef	count_ptr<const param_expr_list>	const_relaxed_args_type;
	typedef	count_ptr<const const_param_expr_list>	
						instance_relaxed_actuals_type;
protected:
	type_ref_ptr_type				type;
	// TODO: switch this bad boy over to the default, 
	// also in chan_traits.h
	const_relaxed_args_type				relaxed_args;
protected:
	instantiation_statement_type_ref_base() :
		type(NULL), relaxed_args() { }

	explicit
	instantiation_statement_type_ref_base(const type_ref_ptr_type& t) :
			type(t), relaxed_args(NULL) { }

	instantiation_statement_type_ref_base(const type_ref_ptr_type& t,
			const const_relaxed_args_type& a) :
			type(t), relaxed_args(a) { }

	type_ref_ptr_type
	get_type(void) const { return type; }

	type_ref_ptr_type
	get_resolved_type(void) const {
		cerr << "FANG, finish channel_type_reference_base::unroll_resolve()!" << endl;
#if 0
		const type_ref_ptr_type ret(type->unroll_resolve());
		if (!ret) {
			type->what(cerr << "ERROR: unable to resolve ") <<
				" during unroll." << endl;
		} else {
			const_cast<this_type*>(this)->type = ret;
			// cache the equivalent resolved type
		}
		return ret;
#endif
		return type_ref_ptr_type(NULL);
	}


	type_ref_ptr_type
	unroll_type_reference(void) const {
		cerr << "FANG, finish channel_type_reference_base::unroll_resolve()!" << endl;
#if 0
		return type->unroll_resolve();
#else
		// temporary
		// don't forget to merge relaxed actuals
		return type_ref_ptr_type(NULL);
#endif
	}

	/**
		Yeah, I know this is incomplete...
	 */
	const_relaxed_args_type
	get_relaxed_actuals(void) const {
		return const_relaxed_args_type(NULL);
	}

	static
	void
	commit_type_first_time(instance_collection_generic_type& v,
			const type_ref_ptr_type& t) {
		v.establish_collection_type(t);
	}

	static
	good_bool
	commit_type_check(instance_collection_generic_type& v,
			const type_ref_ptr_type& t) {
		return v.commit_type(t);
	}

	static
	good_bool
	instantiate_indices_with_actuals(instance_collection_generic_type& v,
			const const_range_list& crl,
			const instance_relaxed_actuals_type& a) {
#if 0
		return v.instantiate_indices(crl, a);
#else
		cerr << "FANG, not ready to instantiate channels yet." << endl;
		return good_bool(false);
#endif
	}

	void
	collect_transient_info_base(persistent_object_manager& m) const {
		type->collect_transient_info(m);
	}

	void
	write_object_base(const persistent_object_manager& m,
			ostream& o) const {
		m.write_pointer(o, type);
	}

	void
	load_object_base(const persistent_object_manager& m,
			istream& i) {
		m.read_pointer(i, type);
	}

};      // end class instantiation_statement_type_ref_base
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INST_STMT_CHAN_H__


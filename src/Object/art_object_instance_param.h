/**
	\file "Object/art_object_instance_param.h"
	Parameter instance collection classes for ART.  
	$Id: art_object_instance_param.h,v 1.19.2.1.2.1 2005/06/12 21:27:59 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INSTANCE_PARAM_H__
#define	__OBJECT_ART_OBJECT_INSTANCE_PARAM_H__

#include "util/boolean_types.h"
#include "Object/art_object_instance_base.h"
#include "Object/art_object_expr_param_ref.h"	// for typedef init_arg_type
#include "util/memory/count_ptr.h"

#include "util/multikey_fwd.h"
#include "util/multikey_qmap_fwd.h"

namespace ART {
namespace entity {
struct pbool_tag;
struct pint_tag;
USING_LIST
using util::memory::count_ptr;
using util::qmap;
using util::multikey_map;
using util::bad_bool;
using util::good_bool;

//=============================================================================
// class instance_collection_base declared in "art_object_instance_base.h"

template <class, size_t>
class value_array;

//=============================================================================
/**
	Instance of a built-in parameter type, such as pint and pbool.  
	Virtualizing.  
	Going to sub-type into pint and pbool.  
	TO DO: derive from a interface for template_argument.  
 */
class param_instance_collection : public instance_collection_base {
private:
	typedef	instance_collection_base	parent_type;
public:
	typedef	parent_type::inst_ref_ptr_type	inst_ref_ptr_type;
	typedef	parent_type::member_inst_ref_ptr_type
						member_inst_ref_ptr_type;

protected:
	param_instance_collection(const size_t d);
public:
	param_instance_collection(const scopespace& o, const string& n, 
		const size_t d);

virtual	~param_instance_collection();

virtual	ostream&
	what(ostream& o) const = 0;

	ostream&
	dump(ostream& o) const;

virtual	bool
	is_partially_unrolled(void) const = 0;

virtual	ostream&
	dump_unrolled_values(ostream& o) const = 0;

virtual	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const = 0;

virtual	count_ptr<meta_instance_reference_base>
	make_meta_instance_reference(void) const = 0;

	/** should just assert fail, forbid reference to param members */
	member_inst_ref_ptr_type
	make_member_meta_instance_reference(const inst_ref_ptr_type& b) const;

	/** appropriate for the context of a template parameter formal */
virtual	count_ptr<const param_expr>
	default_value(void) const = 0;

/**
	A parameter is considered "usable" if it is either initialized
	to another expression, or it is a template formal.  

	Think we may need two flavors of initialization query, 
	because of conservativeness and imprecision:
	"may_be_initialized" and "definitely_initialized"

	TECHNICALITY: what about conditional assignments to a variable?
	Will need assignment expression stack to resolve statically...
	\return true if initialized to an expression.  
	\sa initialize
 */
	bool
	may_be_initialized(void) const;

	bool
	must_be_initialized(void) const;

virtual	good_bool
	assign_default_value(const count_ptr<const param_expr>& p) = 0;

// used by definition_base::certify_template_arguments
virtual	good_bool
	type_check_actual_param_expr(const param_expr& pe) const = 0;

/**
	whether or not this can be resolved to some static constant value.
	Will also need two flavors.  
 */
	bool
	is_static_constant(void) const;

#if 0
NOTE: these functions should only be applicable to simple_param_meta_value_references.  
/**
	Whether or not this parameter is itself a loop index,
	or if indexed, its indices depend on some loop index.
	This may be applicable to instance_collection_base in general.  
 */
	bool is_loop_independent(void) const;

	bool is_unconditional(void) const;
#endif
protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class param_instance_collection

//-----------------------------------------------------------------------------
/**
	A run-time instance of a boolean parameter. 
 */
struct pbool_instance {
public:
	typedef	pbool_value_type		value_type;
public:
	/**
		The unroll-time value of this pbool parameter.
	 */
	value_type	value : 1;
	/**
		Whether or not this instance was truly instantiated,
		Safeguards against extraneous instances in arrays.  
	 */
	bool		instantiated : 1;
	/**	Whether or not value has been initialized exactly 
		once to a value
	 */
	bool		valid : 1;
public:
	pbool_instance() : value(false), instantiated(false), valid(false) { }

	explicit
	pbool_instance(const value_type b) :
		value(b), instantiated(true), valid(false) { }
	// default copy constructor
	// default destructor

	/**
		\return false on error, true on success.  
	 */
	good_bool
	operator = (const value_type b) {
		assert(instantiated);
		if (valid)
			// error: already initialized
			// or allow multiple assignments with the same value?
			return good_bool(false);
		else {
			value = b;
			valid = true;
			return good_bool(true);
		}
	}

};	// end struct pbool_instance

bool
operator == (const pbool_instance& p, const pbool_instance& q);

ostream&
operator << (ostream& o, const pbool_instance& p);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// convenient typedefs

typedef	value_array<pbool_tag,0>	pbool_scalar;
typedef	value_array<pbool_tag,1>	pbool_array_1D;
typedef	value_array<pbool_tag,2>	pbool_array_2D;
typedef	value_array<pbool_tag,3>	pbool_array_3D;
typedef	value_array<pbool_tag,4>	pbool_array_4D;

//=============================================================================
/**
	Run-time instance of integer parameter.  
 */
struct pint_instance {
public:
	typedef pint_value_type		value_type;
public:
	/**
		The unroll-time value of this pint parameter.
	 */
	value_type	value;
	/**
		Whether or not this instance was truly instantiated,
		Safeguards against extraneous instances in arrays.  
	 */
	bool		instantiated : 1;
	/**
		Whether or not value has been initialized exactly 
		once to a value.
	 */
	bool		valid : 1;
public:
	pint_instance() : value(-1), instantiated(false), valid(false) { }

	explicit
	pint_instance(const bool b) :
		value(-1), instantiated(b), valid(false) { }

	explicit
	pint_instance(const value_type v) :
		value(v), instantiated(true), valid(true) { }
	// default copy constructor
	// default destructor

	/**
		\return false on error, true on success.  
	 */
	good_bool
	operator = (const value_type i) {
		assert(instantiated);
		if (valid)
			// error: already initialized
			// or allow multiple assignments with the same value?
			return good_bool(false);
		else {
			value = i;
			valid = true;
			return good_bool(true);
		}
	}

};	// end struct pint_instance

bool
operator == (const pint_instance& p, const pint_instance& q);

ostream&
operator << (ostream& o, const pint_instance& p);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// convenient typedefs

typedef	value_array<pint_tag,0>	pint_scalar;
typedef	value_array<pint_tag,1>	pint_array_1D;
typedef	value_array<pint_tag,2>	pint_array_2D;
typedef	value_array<pint_tag,3>	pint_array_3D;
typedef	value_array<pint_tag,4>	pint_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INSTANCE_PARAM_H__


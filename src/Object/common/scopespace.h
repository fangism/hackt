/**
	\file "Object/common/scopespace.h"
	Classes for scoped objects including namespaces.  
	This file came from "Object/common/scopespace.h"
		in its previous short-lived history.  
	$Id: scopespace.h,v 1.9 2006/01/30 07:41:59 fang Exp $
 */

#ifndef	__HAC_OBJECT_COMMON_SCOPESPACE_H__
#define	__HAC_OBJECT_COMMON_SCOPESPACE_H__

#include <list>
#include "Object/common/util_types.h"
#include "util/persistent.h"		// for persistent object interface
#include "util/boolean_types.h"
#include "util/qmap.h"			// need complete definition
#include "util/hash_qmap.h"		// need complete definition
#include "util/memory/excl_ptr.h"

//=============================================================================
namespace HAC {
namespace parser {
	// note: methods may specify string as formal types, 
	// but you can still pass token_identifiers and token_strings
	// because they are derived from string.
	class token_identifier;
	class qualified_id_slice;
	class qualified_id;
}

namespace entity {
class object;
struct dump_flags;
class definition_base;
class typedef_base;
class instance_collection_base;
class param_value_collection;
using std::list;
using std::string;
using std::istream;
using std::ostream;
using util::hash_qmap;
using util::persistent;
using util::persistent_object_manager;
using parser::token_identifier;
using parser::qualified_id_slice;
using parser::qualified_id;
using util::memory::never_ptr;
using util::memory::some_ptr;
using util::memory::excl_ptr;
using util::qmap;
using util::good_bool;
using util::bad_bool;

//=============================================================================
/**
	Generic base class for scoped entities such as namespaces, 
	process definitions, and other definition bodies.  
	To restrict the type of things that can be added to a specific
	scope-class, derive from this class privately.  
	e.g. process definitions cannot contain other namespaces or other 
	process definitions, and namespaces do not contain formals or
	naked language bodies.  
 */
class scopespace : virtual public persistent {
protected:	// typedefs -- keep these here for re-use

	/**
		Aliased namespaces, which are not owned, 
		cannot be modified.  
	 */
	typedef	qmap<string, never_ptr<const name_space> >	alias_map_type;

	/**
		Container for open namespaces with optional aliases.  
		Doesn't have to be a map because never any need to search
		by key.  List implementation is sufficient, because
		whole list will always be searched, if it is searched at all.  
		These pointers are read-only, and thus not
		owned by this namespace.  
	 */
	typedef list<never_ptr<const name_space> >	namespace_list;

	/**
		This set contains the list of identifiers for this namespace
		that have been mapped to some class: either another namespace, 
		a process/data-type/channel definitions/instantiation.  
		The language currently forbids reuse of identifiers within
		this namespace, so one cannot say namespace x {}; followed
		by int x, even though the syntax tree is sufficient to 
		disambiguate between the uses of x, based on context.  
		We do, however, allow the overshadowing of names from
		other namespaces, say that of the parent, or of an 
		imported or sub-space.  
		This type is needed to rapid lookup of identifiers in a body
		that can refer to a symbol declared in the template formals, 
		port parameters, or local definitions and instantiations.  
		The stored value is a generic polymorphic object pointer 
		whose type is deduced in the grammar.  
		EVERY addition to this namespace must register
		through this hash_map.  
		Again, these pointers are not owned.  
		These (redundantly) stored copies of pointers are read-only.  
		To get the modifiable pointers, you'll need to look them up 
		in the corresponding type-specific map.  
	 */
	typedef	hash_qmap<string, some_ptr<object> >	used_id_map_type;

	// new idea: use used_id_map as cache for type references and 
	// parameters expressions.  
public:
	/**
		Used by footprint.  
	 */
	typedef	used_id_map_type::const_iterator	const_map_iterator;

protected:
	/** convenience struct for dumping */
	class bin_sort {
	// public unary_function<const used_id_map_type::const_iterator&, void>
	public:
		typedef qmap<string, never_ptr<name_space> >
							ns_bin_type;
		typedef qmap<string, never_ptr<definition_base> >
							def_bin_type;
		typedef qmap<string, never_ptr<typedef_base> >
							alias_bin_type;
		typedef qmap<string, never_ptr<instance_collection_base> >
							inst_bin_type;
		typedef qmap<string, never_ptr<param_value_collection> >
							param_bin_type;

		ns_bin_type		ns_bin;
		def_bin_type		def_bin;
		alias_bin_type		alias_bin;
		inst_bin_type		inst_bin;
		param_bin_type		param_bin;

		// only default constructor
	private:
		// prevent accidental copying
		explicit bin_sort(const bin_sort&);
	public:
		bin_sort();
		void operator() (const used_id_map_type::value_type& i);
	};	// end class bin_sort

	/** read-only version of sorted bins */
	class const_bin_sort {
	// public unary_function<const used_id_map_type::const_iterator&, void>
	public:
		typedef qmap<string, never_ptr<const name_space> >
							ns_bin_type;
		typedef qmap<string, never_ptr<const definition_base> >
							def_bin_type;
		typedef qmap<string, never_ptr<const typedef_base> >
							alias_bin_type;
		typedef qmap<string, never_ptr<const instance_collection_base> >
							inst_bin_type;
		typedef qmap<string, never_ptr<const param_value_collection> >
							param_bin_type;

		ns_bin_type		ns_bin;
		def_bin_type		def_bin;
		alias_bin_type		alias_bin;
		inst_bin_type		inst_bin;
		param_bin_type		param_bin;

		// only default constructor
	private:
		// prevent accidental copying
		explicit const_bin_sort(const const_bin_sort&);
	public:
		const_bin_sort();
		void operator() (const used_id_map_type::value_type& i);
		void stats(ostream& o) const;
	};	// end class const_bin_sort

protected:	// members
	// should really only contain instantiations? no definitions?
	// what should a generic scopespace contain?
	/**
		Before mapping a new symbol to a symbolic object, 
		it must not already be mapped to an existing object.  
		All additions to the current scope must register
		through this map.  
		Would be *nice* if code was structured to do this 
		automatically, expecially in sub-classes of scopespace.  
		Making this private forces access through the [] operators.  
	 */
	used_id_map_type	used_id_map;

protected:
	scopespace();
public:
virtual	~scopespace();

#if 0
virtual	ostream&
	what(ostream& o) const = 0;
#endif

virtual	ostream&
	dump(ostream& o) const = 0;

virtual	const string&
	get_key(void) const = 0;

virtual	string
	get_qualified_name(void) const = 0;

virtual	ostream&
	dump_qualified_name(ostream&, const dump_flags&) const = 0;

virtual never_ptr<const scopespace>
	get_parent(void) const = 0;

// horrible temporary hack:
virtual	bool
	is_global_namespace(void) const;

	const_map_iterator
	id_map_begin(void) const { return used_id_map.begin(); }

	const_map_iterator
	id_map_end(void) const { return used_id_map.end(); }

protected:
	ostream&
	dump_for_definitions(ostream&) const;

public:

	never_ptr<const object>
	__lookup_member(const string& id) const;

virtual	never_ptr<const object>
	lookup_member(const string& id) const;

virtual	never_ptr<object>
	lookup_member_with_modify(const string& id) const;

virtual	never_ptr<const object>
	lookup_object(const string& id) const;

virtual	never_ptr<const object>
	lookup_object(const qualified_id_slice& id) const;

virtual	never_ptr<const scopespace>
	lookup_namespace(const qualified_id_slice& id) const;

protected:
	never_ptr<const instance_collection_base>
	add_instance(excl_ptr<instance_collection_base>& i);
public:
	// need id because instantiation statement won't be named yet!
	never_ptr<const instance_collection_base>
	add_instance(const never_ptr<instantiation_statement_base> i, 
		const token_identifier& id, const bool);

	good_bool
	add_definition_alias(const never_ptr<const definition_base> d, 
		const string& a);

	size_t
	exclude_population(void) const;

virtual	bool
	exclude_object(const used_id_map_type::value_type& i) const;

	bool
	exclude_object_val(const used_id_map_type::value_type i) const;

// helper functions for object IO
private:
	// for used_id_map
	void
	collect_used_id_map_pointers(persistent_object_manager& m) const;

	void
	write_object_used_id_map(const persistent_object_manager& m, 
		ostream&) const;

	void
	load_object_used_id_map(const persistent_object_manager& m, 
		istream&);

protected:
	void
	collect_transient_info_base(persistent_object_manager& m) const;

	void
	write_object_base(const persistent_object_manager& m, ostream&) const;

	static
	void
	write_object_base_fake(const persistent_object_manager& m, ostream&);

	void
	load_object_base(const persistent_object_manager& m, istream&);

private:
// no concrete method for loading -- that remains derived-class specific
// so each sub-class may impose its own restrictions
virtual	void
	load_used_id_map_object(excl_ptr<persistent>& o) = 0;
};	// end class scopespace

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

//=============================================================================
#endif	// __HAC_OBJECT_COMMON_SCOPESPACE_H__


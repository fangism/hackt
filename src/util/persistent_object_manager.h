/**
	\file "persistent_object_manager.h"
	Clases related to serial, persistent object management.  
	$Id: persistent_object_manager.h,v 1.12.2.2.2.1 2005/02/18 06:07:45 fang Exp $
 */

#ifndef	__UTIL_PERSISTENT_OBJECT_MANAGER_H__
#define	__UTIL_PERSISTENT_OBJECT_MANAGER_H__

#include <iosfwd>			// include stringstream
#include <vector>
#include "persistent.h"

#include "hash_qmap.h"
#include "memory/pointer_classes.h"
#include "IO_utils.h"			// for read and write to streams

//=============================================================================
// macros

/**
	Default manner for static persistent type registration.  
 */
#define	DEFAULT_PERSISTENT_TYPE_REGISTRATION(T, str)			\
static const util::persistent_traits<T> __persistent_traits_ ## T ## __(str);

//=============================================================================
namespace util {
using std::vector;
using std::istream;
using std::ostream;
using std::ios_base;
using std::streampos;
using std::stringstream;
using std::ofstream;
using std::ifstream;
using namespace util::memory;
using util::hash_qmap;

//=============================================================================
/**
	Persistent object manager class for ART objects.  
	Note that the pointers kept in this structure are just raw pointers, 
	and are never owned.  This manager is never responsible for 
	deleting the memory created.  
 */
class persistent_object_manager {
public:
	/**
		Type for auxiliary construction argument.  
		Should be small like a char for space-efficiency.
	 */
	typedef	char			aux_alloc_arg_type;
private:
	/**
		The class contains the information necessary for reconstructing
		persistent objects associated with a pointer.  
	 */
	class reconstruction_table_entry {
	private:
		/** constant default ios mode */
		static const ios_base::openmode	mode;
	private:
		/** object type enumeration */
		persistent::hash_key	otype;
		/** location of reconstruction, consider object*? */
		const persistent*	recon_addr;

		/**
			Auxiliary allocator argument, useful for 2-level 
			constructor tables.  
		 */
		aux_alloc_arg_type	alloc_arg;

		/** reference count for counter pointers */
	mutable	size_t*			ref_count;
		/**
			scratch flag, general purpose flag,
			consider making mutable
		 */
		bool			scratch;
		/** start of stream position */
		streampos		buf_head;
		/** end of stream position */
		streampos		buf_tail;
		/** stream buffer for temporary storage, count_ptr
			for transferrable semantics with copy assignment */
		count_ptr<stringstream>	buffer;
	public:
	// need default constructor to create an invalid object
		reconstruction_table_entry();

		reconstruction_table_entry(const persistent::hash_key& t, 
			const aux_alloc_arg_type a, 
			const streampos h, const streampos t);

		reconstruction_table_entry(const persistent::hash_key& t, 
			const streampos h, const streampos t);

		reconstruction_table_entry(const persistent* p,
			const persistent::hash_key& t, 
			const aux_alloc_arg_type a);

		// default copy constructor suffices

		~reconstruction_table_entry();

		const persistent::hash_key&
		type(void) const { return otype; }

		const persistent*
		addr(void) const { return recon_addr; }

		aux_alloc_arg_type
		get_alloc_arg(void) const { return alloc_arg; }

		size_t*
		count(void) const;

		void
		assign_addr(persistent* ptr);

		void
		reset_addr();

		void
		flag(void) { scratch = true; }

		void
		unflag(void) { scratch = false; }

		bool
		flagged(void) const { return scratch; }

		stringstream&
		get_buffer(void) const { return *buffer; }

		void
		initialize_offsets(void);

		void
		adjust_offsets(const streampos s);

		streampos
		head_pos(void) const { return buf_head; }

		streampos
		tail_pos(void) const { return buf_tail; }
	};	// end class reconstruction_table_entry

	/**
		Out of paranoia, writing this class to guarantee
		default value of -1.  
	 */
	class Long {
		public:
			long val;
			Long() : val(-1) { }
			Long(const long v) : val(v) { }
			~Long() { }
			operator long () { return val; }
	};	// end class Long

	/**
		Type of map from address to table entry index.  
		Every pointer will be mapped to an auxiliary index.  
		Is the reverse map of reconstruction_table_type.
	 */
	typedef	hash_qmap<const void*, Long>	addr_to_index_map_type;

	/**
		Each persistent object in memory corresponds to one entry
		in this table, whose entries contain information on how to 
		fully reconstruct itself.  
		Is the reverse map of addr_to_index_map_type.  
	 */
	typedef	vector<reconstruction_table_entry>
						reconstruction_table_type;

	/**
		Map from of persistent type's key to allocator function.  
	 */
#if 0
	typedef hash_qmap<persistent::hash_key, reconstruction_functor>
					reconstruction_function_map_type;
#else
	typedef hash_qmap<persistent::hash_key, reconstruct_function_ptr_type>
					reconstruction_function_map_type;
#endif

private:

	/** maps pointer address to entry index */
	addr_to_index_map_type			addr_to_index_map;
	/** entries containing reconstruction information */
	reconstruction_table_type		reconstruction_table;
	/** file position after the header */
	streampos				start_of_objects;
	/** temporary place to keep ownership of root pointer */
	excl_ptr<persistent>			root;

public:
	static bool				dump_reconstruction_table;

public:
	persistent_object_manager();

	~persistent_object_manager();

	/**
		Generic way of registering persistent type.
		May be specialized, of course, where interface is different.  
	 */
	template <class T>
	static
	int
	register_persistent_type(void);

private:
	/**
		The safe accessor to global private static table.  
	 */
	static
	reconstruction_function_map_type&
	reconstruction_function_map(void);

public:
	static
	bool
	verify_registered_type(const persistent::hash_key& k);

	static
	ostream&
	dump_registered_type_map(ostream& o);

// for debugging
	ostream&
	dump_text(ostream& o) const;

// public interface functions to object class hierarchy
	/** pointer registration interface */
	bool
	register_transient_object(
		const persistent* ptr, const persistent::hash_key& t, 
		const aux_alloc_arg_type a = 0);

private:
	bool
	flag_visit(const persistent* ptr);

	ostream&
	lookup_write_buffer(const persistent* ptr) const;

	istream&
	lookup_read_buffer(const persistent* ptr) const;

	long
	lookup_ptr_index(const persistent* ptr) const;

public:
	persistent*
	lookup_obj_ptr(const long i) const;

private:
	size_t*
	lookup_ref_count(const long i) const;

	size_t*
	lookup_ref_count(const persistent* i) const;

private:
	/// private helper method for writing plain pointers
	template <class P>
	void
	__write_pointer(ostream& f, const P& ptr, raw_pointer_tag) const;

	/// private helper method for any pointer class
	template <class P>
	void
	__write_pointer(ostream& f, const P& ptr, pointer_class_base_tag) const;

	/// private helper method for reading plain pointers
	template <class P>
	void
	__read_pointer(istream& f, const P& ptr, raw_pointer_tag) const;

	/// private helper method for reading non-reference-counted pointers
	template <class P>
	void
	__read_pointer(istream& f, const P& ptr, 
		single_owner_pointer_tag) const;

	/// private helper method for reading reference-counted pointers
	template <class P>
	void
	__read_pointer(istream& f, const P& ptr,
		shared_owner_pointer_tag) const;

public:

	// the following template methods are defined in "art_object_IO.tcc"
	/**
		Doesn't actually write out the pointer, but the index 
		representing the object represented by the pointer.
		Precondition: pointer must already be registered.
		\param f output (file) stream.
		\param ptr the pointer (class) object to translate and 
			write out.
	 */
	template <class P>
	void
	write_pointer(ostream& f, const P& ptr) const;

	/**
		ALERT: this intentially and coercively discards const-ness!
		Need to specialize for reference counter pointers!
	 */
	template <class P>
	void
	read_pointer(istream& f, const P& ptr) const;

private:
	/**
		Helper functor for writing sequences of pointers.  
	 */
	class pointer_writer {
	private:
		const persistent_object_manager&	pom;
		ostream& 				os;
	public:
		pointer_writer(const persistent_object_manager& m, 
			ostream& o) : pom(m), os(o) { }

		template <class P>
		void
		operator () (const P& p);
	};	// end class pointer_writer

	/// is this ever used?
	class pointer_reader {
	private:
		const persistent_object_manager&	pom;
		istream& 				is;
	public:
		pointer_reader(const persistent_object_manager& m, 
			istream& i) : pom(m), is(i) { }

		template <class P>
		void
		operator () (const P& p);
	};	// end class pointer_reader

public:
	/**
		Writes a sequence of pointers, mapped to indices.
		Container only needs a simple forward iterator interface.  
	 */
	template <class L>
	void
	write_pointer_list(ostream& f, const L& l) const;

	/**
		Reconstructs a sequence of pointers, mapped to indices.  
		Container only needs a simple forward iterator interface.  
	 */
	template <class L>
	void
	read_pointer_list(istream& f, L& l) const;

#if 0
	/**
		Writes a map of pointers in some order, ignoring the keys.
	 */
	template <class M>
	void
	write_pointer_map(ostream& f, const M<K, P<T> >& l) const;
#endif

	template <class P>
	void
	load_object_once(const P& p) const;

private:
	void
	__load_object_once(persistent* p, raw_pointer_tag) const;

	template <class P>
	void
	__load_object_once(const P& p, pointer_class_base_tag) const;

public:

// two interface functions suffice for file interaction:
	static
	void
	save_object_to_file(const string& s, const persistent& m);

//	template <class T>
	static
	excl_ptr<persistent>
	load_object_from_file(const string& s);

// self-test functions
	static
	excl_ptr<persistent>
	self_test(const string& s, const persistent& m);

	static
	excl_ptr<persistent>
	self_test_no_file(const persistent& m);

private:
	void
	initialize_null(void);

	void
	collect_objects(void);

	void
	load_objects(void);

	void
	write_header(ofstream& f);

	void
	load_header(ifstream& f);

	/** completes table of computed offsets and writes out all buffers */
	void
	finish_write(ofstream& f);

	/** reads in buffer into pieces */
	void
	finish_load(ifstream& f);

	/** just allocate objects without initializing */
	void
	reconstruct(void);

	excl_ptr<persistent>
	get_root(void);

	void
	reset_for_loading(void);

};	// end class persistent_object_manager

//=============================================================================

/**
	Using per-class traits removes the burden of adding
	static const members to persistent classes.  
	This may be specialized for classes that need more than the default
	type registration.  
	To register a type, create one instance of the traits in the source
	file, preferably where the class methods are defined.  
 */
template <class T>
class persistent_traits {
friend class persistent_object_manager;
public:
	typedef	T				type;
private:
	/**
		Unique hash string identifier for a type.  
	 */
	static persistent::hash_key	type_key;

	/**
		Value is determined at run-time, and is dependent on
		the global ordering of static globals, so do not count
		on the number being consistent across compilers
		and platforms.  
	 */
	static int			type_id;

	/**
		Default static method for allocating.  
	 */
	static reconstruct_function_ptr_type reconstructor;

	/**
		To guarantee that type T is derived somehow from persistent.
	 */
	static persistent* null;

public:
	/**
		Construct one static instance of the traits to 
		set the type_key with a designated string.  
		Check to make sure it wasn't already set.
		i.e. only one instance of the traits is allowed globally.  
		\param s the hash string for type T.
	 */
	explicit
	persistent_traits(const string& s);

	// default destructor

};	// end class persistent_traits

//=============================================================================
}	// end namespace util

#endif	//	__UTIL_PERSISTENT_OBJECT_MANAGER_H__


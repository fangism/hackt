/**
	\file "persistent_object_manager.h"
	Clases related to serial, persistent object management.  
	$Id: persistent_object_manager.h,v 1.5 2004/11/02 07:52:17 fang Exp $
 */

#ifndef	__PERSISTENT_OBJECT_MANAGER_H__
#define	__PERSISTENT_OBJECT_MANAGER_H__

#include "persistent.h"

#include "hash_qmap.h"
#include "sstream.h"			// reduce to forward decl?
#include "ptrs.h"			// need complete definition
#include "count_ptr.h"			// need complete definition
#include "art_utils.h"			// for read and write to streams

//=============================================================================
// macros

/**
	Default manner for static persistent type registration.  
 */
#define	DEFAULT_PERSISTENT_TYPE_REGISTRATION(T, str)			\
	const persistent::hash_key T::persistent_type_key(str);		\
	const int T::persistent_type_id = 				\
		persistent_object_manager::register_persistent_type<T>();

//=============================================================================
namespace util {
using namespace std;
using namespace HASH_QMAP_NAMESPACE;
using namespace COUNT_PTR_NAMESPACE;
using namespace PTRS_NAMESPACE;

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
		/** scratch flag, general purpose flag */
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
		size_t*		count(void) const;
		void		assign_addr(persistent* ptr);
		void		reset_addr();
		void		flag(void) { scratch = true; }
		void		unflag(void) { scratch = false; }
		bool		flagged(void) const { return scratch; }
		stringstream&	get_buffer(void) const { return *buffer; }
		void		initialize_offsets(void);
		void		adjust_offsets(const streampos s);
		streampos	head_pos(void) const { return buf_head; }
		streampos	tail_pos(void) const { return buf_tail; }
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
	int register_persistent_type(void);

private:
	/**
		Why naked pointer?  because objects of non-built-in type
		are not guaranteed to be initialized.  
		Built-in types, such as pointers, are guaranteed
		to be initialized before static objects are initialized.  
		The memory is deallocated by wrapping the naked pointer
		with an excl_ptr, the_reconstruction_function_map_ptr_wrapped.  
	 */
	static
	reconstruction_function_map_type*
	the_reconstruction_function_map_ptr;

	/**
		Extra hackery to ensure that the global private static
		naked pointer is properly deleted.  
	 */
	static
	excl_ptr<reconstruction_function_map_type>
	the_reconstruction_function_map_ptr_wrapped;

	/**
		The safe accessor to global private static table.  
	 */
	static
	reconstruction_function_map_type&
	get_reconstruction_function_map(void);

public:
	static
	bool verify_registered_type(const persistent::hash_key& k);

	static
	ostream&
	dump_registered_type_map(ostream& o);

// for debugging
	ostream& dump_text(ostream& o) const;

// public interface functions to object class hierarchy
	/** pointer registration interface */
	bool register_transient_object(
		const persistent* ptr, const persistent::hash_key& t, 
		const aux_alloc_arg_type a = 0);
	bool flag_visit(const persistent* ptr);
	ostream& lookup_write_buffer(const persistent* ptr) const;
	istream& lookup_read_buffer(const persistent* ptr) const;

	long lookup_ptr_index(const persistent* ptr) const;
	persistent*	lookup_obj_ptr(const long i) const;
	size_t*	lookup_ref_count(const long i) const;

	// the following template methods are defined in "art_object_IO.tcc"
	/**
		Doesn't actually write out the pointer, but the index 
		representing the object represented by the pointer.
		Precondition: pointer must already be registered.
		\param f output (file) stream.
		\param ptr the pointer (class) object to translate and 
			write out.
	 */
	template <template <class> class P, class T>
	void write_pointer(ostream& f, const P<T>& ptr) const;

	/**
		ALERT: this intentially and coercively discards const-ness!
		Need to specialize for reference counter pointers!
	 */
	template <template <class> class P, class T>
	void read_pointer(istream& f, const P<T>& ptr) const;

	/**
		Partial specialization of read_pointer for 
		reference-counted pointers.
	 */
	template <class T>
	void read_pointer(istream& f, const count_ptr<T>& ptr) const;

	template <class T>
	void read_pointer(istream& f, const count_const_ptr<T>& ptr) const;

	/**
		Writes a sequence of pointers, mapped to indices.
		Container only needs a simple forward iterator interface.  
	 */
	template <template <class> class L, template <class> class P, class T >
	void write_pointer_list(ostream& f, const L<P<T> >& l) const;

	/**
		Reconstructs a sequence of pointers, mapped to indices.  
		Container only needs a simple forward iterator interface.  
	 */
	template <template <class> class L, template <class> class P, class T >
	void read_pointer_list(istream& f, L<P<T> >& l) const;

	/**
		Writes a map of pointers in some order, ignoring the keys.
	 */
	template <template <class, class> class M, class K, 
		template <class> class P, class T >
	void write_pointer_map(ostream& f, const M<K, P<T> >& l) const;


// two interface functions suffice for file interaction:
	static void	save_object_to_file(const string& s, 
				const persistent& m);

	template <class T>
	static excl_ptr<T>
			load_object_from_file(const string& s);

// self-test functions
	template <class T>
	static excl_ptr<T>
			self_test(const string& s, const T& m);

	template <class T>
	static excl_ptr<T>
			self_test_no_file(const T& m);

private:
	void initialize_null(void);
	void collect_objects(void);
	void load_objects(void);
	void write_header(ofstream& f);
	void load_header(ifstream& f);
	/** completes table of computed offsets and writes out all buffers */
	void finish_write(ofstream& f);
	/** reads in buffer into pieces */
	void finish_load(ifstream& f);
	/** just allocate objects without initializing */
	void reconstruct(void);

	template <class T>
	excl_ptr<T>	get_root(void);
	void reset_for_loading(void);

};	// end class persistent_object_manager

//=============================================================================
}	// end namespace util

#endif	//	__PERSISTENT_OBJECT_MANAGER_H__


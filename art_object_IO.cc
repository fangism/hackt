// "art_object_IO.cc"

#include <iostream>
#include <fstream>
#include "hash_specializations.h"	// include this first
	// for hash specialization to take effect
#include "art_object_IO.h"
#include "art_object.h"			// yes, ALL object classes
#include "art_utils.tcc"
#include "count_ptr.h"

namespace ART {
namespace entity {
//=============================================================================

const reconstruct_function_ptr_type
persistent_object_manager::
reconstruction_function_table[MAX_TYPE_INDEX_ENUM] = {
	NULL, 			// first slot is reserved
	&name_space::construct_empty, 
	// more reconstructors here...
};

// do we need a reloead function table?

//=============================================================================
// class reconstruction_table_entry method definitions

const ios_base::openmode
persistent_object_manager::reconstruction_table_entry::mode = 
	ios_base::in | ios_base::out | ios_base::binary;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

persistent_object_manager::reconstruction_table_entry::
	reconstruction_table_entry() :
		otype(NULL_TYPE), recon_addr(NULL), scratch(false), 
		buf_head(0), buf_tail(0), buffer(new stringstream(mode)) {
	assert(buffer);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent_object_manager::reconstruction_table_entry::
	reconstruction_table_entry(
		const type_index_enum t, 
		const streampos hd, const streampos tl) :
		otype(t), recon_addr(NULL), scratch(false), 
		buf_head(hd), buf_tail(tl), buffer(new stringstream(mode)) {
	assert(buffer);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent_object_manager::reconstruction_table_entry::
	reconstruction_table_entry(
		const object* p, 
		const type_index_enum t) :
		otype(t), recon_addr(p), scratch(false), 
		buf_head(0), buf_tail(0), buffer(new stringstream(mode)) {
	assert(buffer);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent_object_manager::reconstruction_table_entry::
	~reconstruction_table_entry() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
persistent_object_manager::reconstruction_table_entry::assign_addr(
		object* ptr) {
	assert(!recon_addr);
	recon_addr = ptr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just evaluates the size of the buffer, and records the 
	information in the buf_head and buf_tail members.  
	Later, the positions will be adjusted relative to their
	absolute position in the final serialized output stream.  
 */
void
persistent_object_manager::reconstruction_table_entry::initialize_offsets(
		void) {
	buf_head = buffer->tellg();
	buf_tail = buffer->tellp();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
persistent_object_manager::reconstruction_table_entry::adjust_offsets(
		const streampos s) {
	buf_head += s;
	buf_tail += s;
}

//=============================================================================
// class persistent_object_manager method definitions

persistent_object_manager::persistent_object_manager() :
		addr_to_index_map(),
		reconstruction_table(), 
		start_of_objects(0), 
		root(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent_object_manager::~persistent_object_manager() {
	// recall: this class is NOT responsible for releasing
	// the memory referenced by these pointers
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The first non-NULL object is special: it is the root namespace.  
	Returning an excl_ptr guarantees that memory will
	be managed properly.
	When the excl_ptr hits the end of a scope, unless ownership
	has been transferred, the memory should be recursively reclaimed.  
	Thus, this is not a const method.  
 */
excl_ptr<name_space>
persistent_object_manager::get_root_namespace(void) {
	assert(root);		// necessary?
	return root;
	// this relinquishes ownership and responsibility for deleting
	// to whomever consumes the returned excl_ptr
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers a persistent object that will be stored.  
	Returns true if address was already visited.  
 */
bool
persistent_object_manager::register_transient_object(
		const object* ptr, const type_index_enum t) {
	const long probe = addr_to_index_map[ptr];
	if (probe >= 0) {
		// sanity check
		const reconstruction_table_entry& e = 
			reconstruction_table[probe];
		assert(e.type() == t);
		assert(e.addr() == ptr);
		return true;
	} else {
		// else add new entry
		addr_to_index_map[ptr] = reconstruction_table.size();
		reconstruction_table.push_back(
			reconstruction_table_entry(ptr, t));
			// should transfer ownership of newly 
			// constructed stringstream
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initializes entry 0 with NULL.  
 */
void
persistent_object_manager::initialize_null(void) {
	assert(!reconstruction_table.size());
	register_transient_object(NULL, NULL_TYPE);
#if 0
	reconstruction_table_entry& e = reconstruction_table[0];
	e.set_head(0);
	e.set_tail(0);
	assert(!e.head_pos());
	assert(!e.tail_pos());
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Uses the table entry's scratch flag.  
	Sets it to true.  
	\return true if already visited, false if this is first time.
 */
bool
persistent_object_manager::flag_visit(const object* ptr) {
	const long probe = addr_to_index_map[ptr];
	assert(probe >= 0);
	reconstruction_table_entry& e = reconstruction_table[probe];
	assert(e.addr() == ptr);		// sanity check
	if (e.flagged())
		return true;
	else {
		e.flag();
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns an integer index corresponding to this address.  
	\param ptr must be a registered address in this object manager.  
	\return index corresponding to the entry.  
 */
long
persistent_object_manager::lookup_ptr_index(const object* ptr) const {
//	assert(ptr);
	const long probe = addr_to_index_map[ptr];
//	assert(probe >= 0);
	// because uninitialized value of Long is -1
	if (probe < 0) {
		// more useful diagnosis message
		if (ptr)
			ptr->what(cerr << "FATAL: Object (") << ") at addr "
				<< ptr << " has not been registered with "
				"the object manager!" << endl;
		// else just NULL, don't bother
		exit(1);
	}
	return probe;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the pointer corresponding to the indexed
	reconstruction table entry.  
 */
object*
persistent_object_manager::lookup_obj_ptr(const long i) const {
	const reconstruction_table_entry& e = reconstruction_table[i];
	return const_cast<object*>(e.addr());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param address of the object, which maps to some reconstruction
		table entry.  
	\return writeable reference to a stream buffer.  
 */
ostream&
persistent_object_manager::lookup_write_buffer(const object* ptr) const {
	return reconstruction_table[lookup_ptr_index(ptr)].get_buffer();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param address of the object, which maps to some reconstruction
		table entry.  
	\return readable reference to a stream buffer.  
 */
istream&
persistent_object_manager::lookup_read_buffer(const object* ptr) const {
	return reconstruction_table[lookup_ptr_index(ptr)].get_buffer();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
persistent_object_manager::dump_text(ostream& o) const {
	o << "Persistent Object Manager text dump: " << endl;
	long i = 0;
	const long max = reconstruction_table.size();
	o << "\ti\taddr\t\ttype\thead\ttail" << endl;
	for ( ; i < max; i++) {
		const reconstruction_table_entry& e = reconstruction_table[i];
		o << '\t' << i << '\t' << e.addr() << '\t' << e.type() 
			<< '\t' << e.head_pos()
			<< '\t' << e.tail_pos() << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just write out the reconstruction_table to output file stream.
	This should only be called AFTER the header entries are finalized.  
	Not constant because it sets start_of_objects.  
	\param f must be open in output mode with binary mode.  
 */
void
persistent_object_manager::write_header(ofstream& f) {
	// How many entries to expect?
	const size_t max = reconstruction_table.size();
	write_value(f, max);
	// include NULL in entry in write-out
	size_t i = 0;
	for ( ; i<max; i++) {
		const reconstruction_table_entry& e = reconstruction_table[i];
		write_value(f, e.type());
		write_value(f, e.head_pos());
		write_value(f, e.tail_pos());
	}
	write_value(f, -1L);
	start_of_objects = f.tellp();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load the reconstruction_table from the input file stream.  
	\param f must be open in input mode with binary mode.  
 */
void
persistent_object_manager::load_header(ifstream& f) {
	size_t max;
	read_value(f, max);
	size_t i = 0;
	for ( ; i<max; i++) {
		type_index_enum	t;
		streampos head, tail;
		read_value(f, t);
		read_value(f, head);
		read_value(f, tail);
		assert(t >= 0);		// range check
		assert(t < MAX_TYPE_INDEX_ENUM);
		reconstruction_table.push_back(
			reconstruction_table_entry(t, head, tail));
	}
	long neg_one;
	read_value(f, neg_one);
	assert(neg_one == -1L);
	start_of_objects = f.tellg();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This pass just allocates memory for the entries in the
	reconstruction table, and doesn't initialize any of their contents.  
	Also constructs reverse map from address to index.  
 */
void
persistent_object_manager::reconstruct(void) {
	const size_t max = reconstruction_table.size();
	// skip the 0th entry, is reserved NULL
	addr_to_index_map[NULL] = 0;
	size_t i = 1;
	for ( ; i<max; i++) {
		reconstruction_table_entry& e = reconstruction_table[i];
		type_index_enum t = e.type();
		assert(t);
		e.assign_addr((*reconstruction_function_table[t])());
		addr_to_index_map[e.addr()] = i;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Serially writes all registered objects to output stream.
	Not const because the the reconstruction table entries 
	are updated with computed offsets.  
	This should reverse the process of finish_load.
 */
void
persistent_object_manager::finish_write(ofstream& f) {
	// First, compute offsets of the stream segments (buffers)
	const size_t max = reconstruction_table.size();
	size_t i = 0;
	for ( ; i<max; i++) {
		reconstruction_table_entry& e = reconstruction_table[i];
		// flush out each stream buffer in order
		istream& o = e.get_buffer();	// is a stringstream
		assert(o.good());
		stringbuf* sb = IS_A(stringbuf*, o.rdbuf());
		assert(sb);
		const int size = sb->in_avail();	// characters available
		assert(size == e.tail_pos() -e.head_pos());
		const string str(sb->str());
		f.write(str.c_str(), size);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visits each registered object and write the segment of its
	output to its respective buffer in the reconstruction table.  
	Also initializes the buf_head and buf_tail relative offsets.  
	This should reverse the process of load_objects.
 */
void
persistent_object_manager::collect_objects(void) {
	const size_t max = reconstruction_table.size();
	size_t i = 1;			// don't initialize the 0th one!
//	dump_text(cerr);		// DEBUG
	for ( ; i<max; i++) {
		reconstruction_table_entry& e = reconstruction_table[i];
		object* o = const_cast<object*>(e.addr());
		assert(o);
		o->write_object(*this);	// virtual
		e.initialize_offsets();
	}
	// next pass: translate offsets into absolute positions.  
	i = 0;				// or start at 1, no difference
	streampos tail = 0;
	for ( ; i<max; i++) {
		reconstruction_table_entry& e = reconstruction_table[i];
		streampos len = e.tail_pos();
		e.adjust_offsets(tail);
		tail += len;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	After reading the header, this continues to deserialize
	the objects that follow, loading them into local buffers
	in each reconstruction_table_entry.  
	Recall that the head and tail offsets in the reconstruction table
	are relative to the start of the object section.  
	This should reverse the process of finish_write.
	After this procedure, can close the file.  
	\param f the input file stream.  
 */
void
persistent_object_manager::finish_load(ifstream& f) {
	const size_t max = reconstruction_table.size();
	// skip the 0th object, it's reserved NULL
	size_t i = 1;
	for ( ; i<max; i++) {
		reconstruction_table_entry& e = reconstruction_table[i];
		const int size = e.tail_pos() -e.head_pos();
		f.seekg(e.head_pos() +start_of_objects);
		ostream& o = e.get_buffer();
		assert(o.good());
		char* cbuf = new char [size];
		assert(cbuf);
		// is there a better way to do this, eliminating intermediate?
		f.read(cbuf, size);
		o.write(cbuf, size);
		delete [] cbuf;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This should reverse the process of collect_objects.
	After this process, all registered objects should be coherent!
	Calls virtual function for initializing objects.  
	At the end, assumes responsibility for deleting everything
	by wrapping the root namespace pointer.  
 */
void
persistent_object_manager::load_objects(void) {
	const size_t max = reconstruction_table.size();
	size_t i = 1;		// 0th object is reserved NULL, skip it
	for ( ; i<max; i++) {
		reconstruction_table_entry& e = reconstruction_table[i];
		object* o = const_cast<object*>(e.addr());
		assert(o);
		o->load_object(*this);
		// virtual call, unavoidable const cast
	}
	// Finally, after this is called, immediately assume responsibility
	// for deleting all memory, by wrapping the root pointer
	// to the global namespace in an excl_ptr
	// Entry at position 1 is always the root namespace, global.  
	object* r = lookup_obj_ptr(1);
	name_space* ns = IS_A(name_space*, r);
	assert(ns);
	root = excl_ptr<name_space>(ns);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Saves entire hierarchical object collection to file.  
 */
void
persistent_object_manager::save_object_to_file(const string& s, 
		never_const_ptr<name_space> g) {
	assert(g);
	ofstream f(s.c_str(), ios_base::binary | ios_base::trunc);
	persistent_object_manager pom;
	pom.initialize_null();			// reserved 0th entry
	g->collect_transient_info(pom);		// recursive visitor
	pom.collect_objects();			// buffers output in segments
//	pom.dump_text(cerr << endl) << endl;	// for debugging
	pom.write_header(f);
	pom.finish_write(f);			// serialize
	f.close();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Loads hierarchical object collection from file.  
 */
excl_ptr<name_space>
persistent_object_manager::load_object_from_file(const string& s) {
	ifstream f(s.c_str(), ios_base::binary);
	persistent_object_manager pom;
	// don't initialize_null, will be loaded in from table
	pom.load_header(f);
	pom.finish_load(f);
	f.close();                              // done with file
	pom.reconstruct();                      // allocate-only pass
//	pom.dump_text(cerr << endl) << endl;	// debugging only
	// Oh no, partially initialized objects!
	// Set their values before anyone observes them!
	pom.load_objects();
	// must acquire root object in some owned pointer!
	return pom.get_root_namespace();
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART


// "art_object.h"

#ifndef	__ART_OBJECT_H__
#define	__ART_OBJECT_H__

#include <list>
#include <map>

#include "art_macros.h"
#include "art_parser.h"

using namespace std;

namespace ART {
namespace entity {
//=============================================================================
// forward declarations

//=============================================================================
class object {

};

//=============================================================================
class name_space {

};

//=============================================================================
class definition : public object {

};

//=============================================================================
class instantiation : public object {

};

//=============================================================================
class process_definition : public definition {

};

//=============================================================================
class process_instantiation : public instantiation {

};

//=============================================================================
/// abstract base class for types and their representations
class type_definition : public definition {

};

//-----------------------------------------------------------------------------
class built_int_type_def : public type_definition {

};

//-----------------------------------------------------------------------------
// no formal name, just type name
class user_type_def : public type_definition {
private:
	typedef	list<const type_definition*>	type_members;
protected:
	string			key;		///< name of type
	// list of other type definitions
	type_members		template_params;
	type_members		members;
public:
	user_type_def(const parser::token_string& name) : type_definition(), 
		key(name), members() { }
virtual	~user_type_def() { }

};

//=============================================================================
class type_instantiation : public instantiation {
protected:
	const type_definition*	type;
	string			key;		///< name of instance
public:


};

//=============================================================================
// built-in types will be added to the global scope


//=============================================================================
// built-in types need to be statically initialized

// static type_definition("int")

//=============================================================================
};
};

#endif	// __ART_OBJECT_H__


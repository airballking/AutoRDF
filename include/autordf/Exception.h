#ifndef AUTORDF_EXCEPTION_H
#define AUTORDF_EXCEPTION_H

#include <exception>
#include <string>

namespace autordf {

class Exception : public std::exception {
public:
    // Constructor with a given error string
    Exception( const std::string& description );

    virtual ~Exception() throw();

    // Returns error information.
    virtual const char* what() const throw();

private:
    std::string _description;
};

#define AUTORDFEXCEPTION(exname) \
class exname : public Exception { \
public: \
    exname( const std::string& description ) : Exception(#exname + std::string(": ") + description) {} \
};

/**
 * Mandatory property for the object is not found
 */
AUTORDFEXCEPTION(PropertyNotFound)

/**
 * Current object is not of the expected type
 */
AUTORDFEXCEPTION(InvalidClass)

/**
 * Current object is not an instance of the expected enum
 */
AUTORDFEXCEPTION(InvalidEnum)

/**
 * Property is instanciated more than once for current object
 */
AUTORDFEXCEPTION(DuplicateProperty)

/**
 * IRI is invalid
 */
AUTORDFEXCEPTION(InvalidIRI)

/**
 * Unrecognize file format: the input file is not in a RDF format we know about
 */
AUTORDFEXCEPTION(UnsupportedRdfFileFormat)

/**
 * Unable to read/write to/from file
 */
AUTORDFEXCEPTION(FileIOError)

/**
 * e.g. Invalid cast as a Resource node
 */
AUTORDFEXCEPTION(InvalidNodeType)

/**
 * Unable to convert RDF data to C++ data type
 */
AUTORDFEXCEPTION(DataConvertionFailure)

/**
 * Inside bug or misuse of our API
 */
AUTORDFEXCEPTION(InternalError)
}

#endif //AUTORDF_EXCEPTION_H

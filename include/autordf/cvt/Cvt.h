#ifndef AUTORDF_CVT_CVT_H
#define AUTORDF_CVT_CVT_H

#include <string>
#include <stdexcept>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/date_time.hpp>

namespace autordf {
namespace cvt {

#define CVT_TYPES_DEF(X) \
    X(xsd_string) \
    X(xsd_boolean) \
    X(xsd_decimal) \
    X(xsd_float) \
    X(xsd_double) \
    X(xsd_dateTime) \
    X(xsd_integer) \
    X(xsd_dateTimeStamp) \
    X(xsd_nonNegativeInteger) \
    X(xsd_positiveInteger) \
    X(xsd_nonPositiveInteger) \
    X(xsd_negativeInteger) \
    X(xsd_long) \
    X(xsd_unsignedLong) \
    X(xsd_int) \
    X(xsd_unsignedInt) \
    X(xsd_short) \
    X(xsd_unsignedShort) \
    X(xsd_byte) \
    X(xsd_unsignedByte)

// Integer types are a pain. Mapping from/to C++ <--> XML Schema is done using info from
// - http://en.cppreference.com/w/cpp/language/types
// and
// - http://www.w3.org/TR/xmlschema-2
//
// 1 - We make sure that data types association between both world are at least big enough to be sure that
// any data read out there will fit in the C++ type, except for xsd:decimal and xsd:*integer, where we merely rely on an approximation
// We approximate xsd:decimal with double and xsd:*integer with long long.
// 2 - However it is responsibility of the user to make sure
// that the data he puts in his C++ variables will fit in the corresponding XSD types

enum class RdfTypeEnum {
#define X(a) a,
   CVT_TYPES_DEF(X)
#undef X
};

inline std::string trim(const std::string &s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && isspace(*it))
        it++;

    std::string::const_reverse_iterator rit = s.rbegin();
    while (rit.base() != it && isspace(*rit))
        rit++;

    return std::string(it, rit.base());
}

inline std::string rdfTypeEnumString(RdfTypeEnum enumVal) {
    static const std::string RAWVALS[] = {
#define X(a) #a,
    CVT_TYPES_DEF(X)
#undef X
    };
    std::string val = RAWVALS[static_cast<int>(enumVal)];
    for ( char& c : val) {
        if ( c == '_' ) {
            c = ':';
        }
    }
    return val;
}

inline std::string getLeft(const std::string& rawValue) {
    auto pos = rawValue.find('^');
    if ( pos != std::string::npos ) {
        return rawValue.substr(1, pos - 2);
    } else {
        throw std::runtime_error("Unable to convert value as ^^ was not found in: " + rawValue);
    }
}

template<typename CppType, RdfTypeEnum rdfType> class toCpp;

template<typename CppType> CppType toCppGeneric(const std::string& rawValue) {
    return boost::lexical_cast<CppType>(trim(getLeft(rawValue)));
}

inline boost::posix_time::ptime toCppDateTime(const std::string& rawValue) {
    std::istringstream ss(trim(getLeft(rawValue)));
    ss.exceptions(std::ios_base::failbit);
    boost::posix_time::time_input_facet* input_facet = new boost::posix_time::time_input_facet;
    input_facet->format("%Y-%m-%dT%H:%M:%S%F%Q");
    ss.imbue(std::locale(std::locale::classic(), input_facet));
    boost::local_time::local_date_time result(boost::date_time::not_a_date_time);
    ss >> result;
    return result.utc_time();
};

#define GENERIC_TOCPP(cppType, rdfType) \
template<> class toCpp<cppType, RdfTypeEnum::rdfType> { \
public: \
    static cppType val(const std::string& rawValue) { return toCppGeneric<cppType>(rawValue); } \
}; \

// xs:string
template<RdfTypeEnum rdfType> class toCpp<std::string, rdfType> {
public:
    static std::string val(const std::string& rawValue) {
        return getLeft(rawValue);
    }
};

// xsd:integer
GENERIC_TOCPP(long long int, xsd_integer)

// xsd:decimal
GENERIC_TOCPP(double, xsd_decimal)

// xsd:float
GENERIC_TOCPP(float, xsd_float)

// xsd:double
GENERIC_TOCPP(double, xsd_double)

// xsd_boolean
template<typename CppType> class toCpp<CppType, RdfTypeEnum::xsd_boolean> {
public:
    static CppType val(const std::string& rawValue) {
        std::string left = trim(getLeft(rawValue));
        if ( left == "true" || left == "1") {
            return true;
        } else if ( left == "false" || left == "0") {
            return false;
        } else {
            throw std::runtime_error("Invalid value '" + left + "' for boolean");
        }
    }
};

// xsd:dateTime
template<> class toCpp<boost::posix_time::ptime, RdfTypeEnum::xsd_dateTime> {
public:
    static boost::posix_time::ptime val(const std::string& rawValue) {
        return toCppDateTime(rawValue);
    }
};

// xsd:dateTimeStamp
template<> class toCpp<boost::posix_time::ptime, RdfTypeEnum::xsd_dateTimeStamp> {
public:
    static boost::posix_time::ptime val(const std::string& rawValue) {
        return toCppDateTime(rawValue);
    }
};

// xsd:nonNegativeInteger
GENERIC_TOCPP(long long unsigned int, xsd_nonNegativeInteger)

// xsd:positiveInteger
GENERIC_TOCPP(long long unsigned int, xsd_positiveInteger)

// xsd:nonPositiveInteger
GENERIC_TOCPP(long long int, xsd_nonPositiveInteger)

// xsd:negativeInteger
GENERIC_TOCPP(long long int, xsd_negativeInteger)

// xsd:long
GENERIC_TOCPP(long long int, xsd_long)

// xsd:unsignedLong
GENERIC_TOCPP(long long unsigned int, xsd_unsignedLong)

// xsd:int
GENERIC_TOCPP(long int, xsd_int)

// xsd:unsignedInt
GENERIC_TOCPP(long unsigned int, xsd_unsignedInt)

// xsd:short
GENERIC_TOCPP(short, xsd_short)

// xsd:unsignedShort
GENERIC_TOCPP(unsigned short, xsd_unsignedShort)

// xsd:byte
template<> class toCpp<char, RdfTypeEnum::xsd_byte> {
public:
    static char val(const std::string& rawValue) {
        return boost::lexical_cast<short>(trim(getLeft(rawValue)));
    }
};

// xsd:unsignedByte
template<> class toCpp<unsigned char, RdfTypeEnum::xsd_unsignedByte> {
public:
    static unsigned char val(const std::string& rawValue) {
        return boost::lexical_cast<unsigned short>(trim(getLeft(rawValue)));
    }
};

template<typename CppType, RdfTypeEnum rdfType> class toRdf;

template<RdfTypeEnum rdfType, typename CppType> std::string toRdfGeneric(const CppType& cppValue) {
    std::ostringstream ss;
    ss << "\"" << cppValue << "\"^^" << rdfTypeEnumString(rdfType);
    return ss.str();
};

#define GENERIC_TORDF(cppType, rdfType) \
template<> class toRdf<cppType, RdfTypeEnum::rdfType> { \
public: \
    static std::string val(const cppType& cppValue) { return toRdfGeneric<RdfTypeEnum::rdfType>(cppValue); } \
};

// xsd:string
GENERIC_TORDF(std::string, xsd_string)

// xsd:integer
GENERIC_TORDF(long long int, xsd_integer)

// xsd:decimal
GENERIC_TORDF(double, xsd_decimal)

// xsd:float
GENERIC_TORDF(float, xsd_float)

// xsd:double
GENERIC_TORDF(double, xsd_double)

// xsd:boolean
template<typename CppType> class toRdf<CppType, RdfTypeEnum::xsd_boolean>  {
public:
    static std::string val(const CppType& cppValue) {
        return std::string("\"") + (cppValue ? "true" : "false") + "\"^^" + rdfTypeEnumString(RdfTypeEnum::xsd_boolean);
    }
};

inline std::string toRdfDateTime(const boost::posix_time::ptime& time, RdfTypeEnum type) {
    std::ostringstream ss;
    ss.exceptions(std::ios_base::failbit);
    boost::posix_time::time_facet* facet = new boost::posix_time::time_facet;
    // ISO Extended with Z as forced timezone
    facet->format("%Y-%m-%dT%H:%M:%S%FZ");
    ss.imbue(std::locale(std::locale::classic(), facet));
    ss << "\"" << time << "\"^^" << rdfTypeEnumString(type);
    return ss.str();
}

// xsd:dateTime
template<> class toRdf<boost::posix_time::ptime, RdfTypeEnum::xsd_dateTime> {
public:
    static std::string val(const boost::posix_time::ptime& time) {
        return toRdfDateTime(time, RdfTypeEnum::xsd_dateTime);
    }
};

// xsd:dateTimeStamp
template<> class toRdf<boost::posix_time::ptime, RdfTypeEnum::xsd_dateTimeStamp> {
public:
    static std::string val(const boost::posix_time::ptime& time) {
        return toRdfDateTime(time, RdfTypeEnum::xsd_dateTimeStamp);
    }
};

// xsd:nonNegativeInteger
GENERIC_TORDF(long long unsigned int, xsd_nonNegativeInteger)

// xsd:positiveInteger
GENERIC_TORDF(long long unsigned int, xsd_positiveInteger)

// xsd:nonPositiveInteger
GENERIC_TORDF(long long int, xsd_nonPositiveInteger)

// xsd:negativeInteger
GENERIC_TORDF(long long int, xsd_negativeInteger)

// xsd:long
GENERIC_TORDF(long long int, xsd_long)

// xsd:unsignedLong
GENERIC_TORDF(long long unsigned int, xsd_unsignedLong)

// xsd:int
GENERIC_TORDF(long int, xsd_int)

// xsd:unsignedInt
GENERIC_TORDF(long unsigned int, xsd_unsignedInt)

// xsd:short
GENERIC_TORDF(short, xsd_short)

// xsd:unsignedShort
GENERIC_TORDF(unsigned short, xsd_unsignedShort)

// xsd:byte
template<> class toRdf<char, RdfTypeEnum::xsd_byte>  {
public:
    static std::string val(char cppValue) {
        std::ostringstream ss;
        ss << "\"" << static_cast<short>(cppValue) << "\"^^" << rdfTypeEnumString(RdfTypeEnum::xsd_byte);
        return ss.str();
    }
};

// xsd:unsignedByte
template<> class toRdf<unsigned char, RdfTypeEnum::xsd_unsignedByte>  {
public:
    static std::string val(unsigned char cppValue) {
        std::ostringstream ss;
        ss << "\"" << static_cast<unsigned short>(cppValue) << "\"^^" << rdfTypeEnumString(RdfTypeEnum::xsd_unsignedByte);
        return ss.str();
    }
};
}
}
#endif //AUTORDF_CVT_CVT_H

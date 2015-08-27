#ifndef AUTORDF_OBJECT_H
#define AUTORDF_OBJECT_H

#include <memory>
#include <list>
#include <iosfwd>
#include <stdexcept>
#include <map>
#include <set>

#include <autordf/Factory.h>
#include <autordf/PropertyValue.h>
#include <autordf/Resource.h>
#include <autordf/ResourceList.h>
#include <autordf/Exception.h>

namespace autordf {

class Factory;

/**
 * Common Ancestor to all AutoRDF generated classes
 * When copied, Objects are implicitly shared, with backend storage
 * provided by librdf.
 * New objects can be created in two ways:
 *  - Using Object(const std::string& iri, const std::string& rdfTypeIRI = "") constructor: Creates a new object in Model
 *  - Using the clone(const std::string& iri) method: Makes a copy of object and write it to Model
 */
class Object {
public:
    /**
     * Full namespace for RDF, including #
     */
    static const std::string RDF_NS;

    /**
     * All newly Objects will be created in this Model
     */
    static void setFactory(Factory *f);

    /**
     * Creates new object, to given iri.
     * @param object IRI. If empty, creates an anonymous (aka blank) object
     * @param rdfTypeIRI. If not empty, will write rdf type property when object is written
     * @param rtti. Rdf Runtime type info, usually auto-generated. If the underlying resource has a type, makes sure it is compatible with
     *   rdfTypeIRI, using rtti type system
     * @throw InvalidClass if the Object is not of type rdfTypeIRI or one of its subclasses may be thrown only if both rdfTypeIRI and rtti are
     * not empty
     */
    Object(const std::string& iri = "", const std::string& rdfTypeIRI = "", const std::map<std::string, std::set<std::string> >* rtti = nullptr);

    /**
     * Build us using the same underlying resource as the other object
     * @param rdfTypeIRI. If not empty, will write rdf type property when object is written
     * @throw InvalidClass if the Object is not of type rdfTypeIRI or one of its subclasses may be thrown only if both rdfTypeIRI and rtti are
     * not empty
     */
    Object(const Object& obj, const std::string& rdfTypeIRI = "", const std::map<std::string, std::set<std::string> >* rtti = nullptr);

    /**
     * Return object iri, or empty if it is a blank node
     */
    const std::string& iri() const;

    /**
     * Gets given property as Object
     * Property should be set.
     * @throw PropertyNotFound if property not set
     * @throw DuplicateProperty if property contains more than one value
     */
    Object getObject(const std::string& propertyIRI) const;

    /**
     * Returns given property as Object.
     * @returns pointer if property exists, null otherwise
     * @throw DuplicateProperty if property contains more than one value
     */
    std::shared_ptr<Object> getOptionalObject(const std::string& propertyIRI) const;

    /**
     * Returns the list of object. If no object found returns empty list
     */
    std::list<Object> getObjectList(const std::string& propertyIRI) const;

    /**
     * Sets object to given property
     */
    void setObject(const std::string& propertyIRI, const Object& obj);

    /**
     * Sets list of objects to given property
     */
    void setObjectList(const std::string& propertyIRI, const std::list<Object>& values);

    /**
     * Gets given property value
     * Property should be set.
     * @throw PropertyNotFound if property not set
     * @throw DuplicateProperty if property contains more than one value
     */
    PropertyValue getPropertyValue(const std::string& propertyIRI) const;

    /**
     * Returns given property as Object.
     * @param propertyIRI Internationalized Resource Identifiers property to query
     * @returns pointer if property exists, null otherwise
     * @throw DuplicateProperty if property contains more than one value
     */
    std::shared_ptr<PropertyValue> getOptionalPropertyValue(const std::string& propertyIRI) const;

    /**
     * Returns the list of the values. If no value are found returns empty list
     * @param propertyIRI Internationalized Resource Identifiers property to query
     */
    std::list<PropertyValue> getPropertyValueList(const std::string& propertyIRI) const;

    /**
     * Erases all previous values for property, and write unique value on place
     * @param propertyIRI Internationalized Resource Identifiers property to set
     * @param val unique value for property
     */
    void setPropertyValue(const std::string& propertyIRI, const PropertyValue& val);

    /**
     * Erases all previous values for property, and write value list on place
     * @param propertyIRI Internationalized Resource Identifiers property to set
     * @param values the list of values. All previous values are removed, and replaced with the given lists
     */
    void setPropertyValueList(const std::string& propertyIRI, const std::list<PropertyValue>& values);

    /**
     * Adds value to this property, preserving all previous values;
     * @param propertyIRI Internationalized Resource Identifiers property to set
     * @param val value for property. This new value is added to the list of values for the Property
     */
    void addPropertyValue(const std::string& propertyIRI, const PropertyValue& val);

    /**
     * Returns true if this object is also of the specified type IRI.
     * @return true If object type attribute contains typeIRI
     * @return false If type attribute does not contains typeIRI
     */
    bool isA(const std::string& typeIRI) const;

    /**
     * Provides ultra-fast trans-typing to another Object descendant
     * This is syntaxic sugar to say myobj.as<T>() instead of T(myobj)
     * @return transtyped object
     */
    template<typename T> T as() {
        return T(*this);
    }

    /**
     * Provides ultra-fast trans-typing to another Object descendant
     * This is syntaxic sugar to say myobj.as<T>() instead of T(myobj)
     * @return transtyped object
     */
    template<typename T> const T as() const {
        return T(*this);
    }

    /**
     * As the name suggests, removes all data
     * Associated with this object.
     * This only deletes properties and links to other objects,
     * It does not delete the objects we point to
     */
    void remove();

    /**
     * Copies this object, to given iri. If iri empty,
     * creates an anonymous (aka blank) object
     */
    Object clone(const std::string& iri = "");

    /**
     * Returns all Objects matching type specified as IRI
     * @param propertyIRI Internationalized Resource Identifiers of the type to be retireved
     */
    static std::list<Object> findByType(const std::string& typeIRI = "");

    /**
     * Dumps objects content to stream
     * @param recurse: if > 0, go down resource hierarchy by recurse level
     * @param indentLevel: How much layer of tabs to insert
     */
    std::ostream& printStream(std::ostream&, int recurse = 0, int indentLevel = 0) const;

    /**
     * Offered to interfaces
     */
    template<typename T> static std::list<T> findHelper(const std::string& iri) {
        const ResourceList& rl = _factory->findByType(iri);
        std::list<T> objList;
        for(const Resource& r : rl) {
            objList.push_back(T(r));
        }
        return objList;
    }

    /**
     * Offered to interfaces
     * @throw InvalidIRI if propertyIRI is empty
     */
    template<typename T> std::list<T> getObjectListImpl(const std::string& propertyIRI) const {
        if ( propertyIRI.empty() ) {
            throw InvalidIRI("Calling getObjectListImpl() with empty IRI is forbidden");
        }
        std::list<T> objList;
        const std::list<Property>& propList = _r.getPropertyValues(propertyIRI);
        for (const Property& prop: propList) {
            objList.push_back(T(prop.asResource()));
        }
        return objList;
    }

    /**
     * Offered to interfaces
     * @throw InvalidIRI if propertyIRI is empty
     */
    template<cvt::RdfTypeEnum rdftype, typename T> std::list<T> getValueListImpl(const std::string& propertyIRI) const {
        if ( propertyIRI.empty() ) {
            throw InvalidIRI("Calling getValueListImpl() with empty IRI is forbidden");
        }
        std::list<T> valueList;
        const std::list<Property>& propList = _r.getPropertyValues(propertyIRI);
        for (const Property& prop: propList) {
            valueList.push_back(prop.value().get<rdftype, T>());
        }
        return valueList;
    }

private:
    /**
     * The resource this object is based on
     */
    Resource _r;

    /**
     * The factory this object belongs to
     */
    static Factory *_factory;

    /**
     * True if rdf type value has already be written
     */
    bool _rdfTypeWritingRequired;
    /**
     * Lazy Type contains the (optional) rdf type for the object
     * Type writing is delayed until first object property is written in database
     */
    std::string _rdfTypeIRI;

    Object(Resource r, const std::string& rdfTypeIRI = "");

    /**
     * Checks if underlying resource is of given type
     * If not add it to the types list
     */
    void addRdfTypeIfNeeded();

    /**
     * Shared constructor
     */
    void construct(const std::string& rdfTypeIRI);

    /**
     * Checks if current object is of rdf type that is either _rdfTypeIRI, or one of its subclasses
     * @param rdfTypesInfo, the types inferred for current class hierarchy
     */
    void runtimeTypeCheck(const std::map<std::string, std::set<std::string> > *rdfTypesInfo) const;
};

/**
 * Writes object in json format.
 *
 * Is equivalent to printStream(os, 0, 0)
 */
std::ostream& operator<<(std::ostream& os, const Object&);
}

#endif //AUTORDF_OBJECT_H

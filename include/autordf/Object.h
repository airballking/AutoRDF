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
#include <autordf/Exception.h>
#include <autordf/Uri.h>

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
     * @param iri object IRI. If empty, creates an anonymous (aka blank) object
     * @param rdfTypeIRI If not empty, will write rdf type property when object is written
     * @param rtti Rdf Runtime type info, usually auto-generated. If the underlying resource has a type, makes sure it is compatible with
     *   rdfTypeIRI, using rtti type system
     * @throw InvalidClass if the Object is not of type rdfTypeIRI or one of its subclasses may be thrown only if both rdfTypeIRI and rtti are
     * not empty
     */
    Object(const Uri& iri = "", const Uri& rdfTypeIRI = "", const std::map<std::string, std::set<std::string> >* rtti = nullptr);

    /**
     * Build us using the same underlying resource as the other object
     *
     * This constructs a new C++ object that points to the same RDF data
     *
     * @param obj the object to use
     * @param rdfTypeIRI If not empty, will write rdf type property when object is written
     * @param rtti Rdf Runtime type info, usually auto-generated. If the underlying resource has a type, makes sure it is compatible with
     *   rdfTypeIRI, using rtti type system
     * @throw InvalidClass if the Object is not of type rdfTypeIRI or one of its subclasses may be thrown only if both rdfTypeIRI and rtti are
     * not empty
     */
    Object(const Object& obj, const Uri& rdfTypeIRI = "", const std::map<std::string, std::set<std::string> >* rtti = nullptr);

    /**
     * Return object iri, or empty if it is a blank node
     */
    const Uri& iri() const;

    /**
     * Return types of current object.
     * If namespaceFilter is set, only types in this namespace will be returned
     * @param namespaceFilter A simple namespace
     */
    std::list<Uri> getTypes(const std::string& namespaceFilter = "") const;

    /**
     * Gets given property as Object
     * Property should be set.
     * @throw PropertyNotFound if property not set
     * @throw DuplicateProperty if property contains more than one value
     */
    Object getObject(const Uri& propertyIRI) const;

    /**
     * Returns given property as Object.
     * @returns pointer if property exists, null otherwise
     * @throw DuplicateProperty if property contains more than one value
     */
    std::shared_ptr<Object> getOptionalObject(const Uri& propertyIRI) const;

    /**
     * Returns the list of object. If no object found returns empty list
     */
    std::list<Object> getObjectList(const Uri& propertyIRI) const;

    /**
     * Sets object to given property
     */
    void setObject(const Uri& propertyIRI, const Object& obj);

    /**
     * Adds object to given property
     * @param propertyIRI Internationalized Resource Identifiers property to set
     * @param obj object to add to the propertyIRI property
     */
    void addObject(const Uri& propertyIRI, const Object& obj);

    /**
     * Sets list of objects to given property
     */
    void setObjectList(const Uri& propertyIRI, const std::list<Object>& values);

    /**
     * Gets given property value
     * Property should be set.
     * @throw PropertyNotFound if property not set
     * @throw DuplicateProperty if property contains more than one value
     */
    PropertyValue getPropertyValue(const Uri& propertyIRI) const;

    /**
     * Returns given property as Object.
     * @param propertyIRI Internationalized Resource Identifiers property to query
     * @returns pointer if property exists, null otherwise
     * @throw DuplicateProperty if property contains more than one value
     */
    std::shared_ptr<PropertyValue> getOptionalPropertyValue(const Uri& propertyIRI) const;

    /**
     * Returns the list of the values. If no value are found returns empty list
     * @param propertyIRI Internationalized Resource Identifiers property to query
     */
    std::list<PropertyValue> getPropertyValueList(const Uri& propertyIRI) const;

    /**
     * Erases all previous values for property, and write unique value on place
     * @param propertyIRI Internationalized Resource Identifiers property to set
     * @param val unique value for property
     */
    void setPropertyValue(const Uri& propertyIRI, const PropertyValue& val);

    /**
     * Erases all previous values for property, and write value list on place
     * @param propertyIRI Internationalized Resource Identifiers property to set
     * @param values the list of values. All previous values are removed, and replaced with the given lists
     */
    void setPropertyValueList(const Uri& propertyIRI, const std::list<PropertyValue>& values);

    /**
     * Adds value to this property, preserving all previous values;
     * @param propertyIRI Internationalized Resource Identifiers property to set
     * @param val value for property. This new value is added to the list of values for the Property
     */
    void addPropertyValue(const Uri& propertyIRI, const PropertyValue& val);

    /**
     * Remove the first value matching val for this property
     * @param propertyIRI Internationalized Resource Identifiers property to remove one value from
     * @param val value to remove
     * @throw PropertyNotFound if propertyIRI has not val as value
     */
    void removePropertyValue(const Uri& propertyIRI, const PropertyValue& val);

    /**
     * Returns true if this object is also of the specified type IRI.
     * @return true If object type attribute contains typeIRI
     * @return false If type attribute does not contains typeIRI
     */
    bool isA(const Uri& typeIRI) const;

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
    Object clone(const Uri& iri = "");

    /**
     * Returns all Objects matching type specified as IRI
     *
     * @param typeIRI Internationalized Resource Identifiers of the type to be retrieved
     */
    static std::list<Object> findByType(const Uri& typeIRI = "");

    /**
     * Returns the only Object with property key set to value
     *
     * @param propertyIRI Internationalized Resource Identifier for the property
     * @param value value to look for
     *
     * @throw DuplicateObject if more than one object have the same property value
     * @throw ObjectNotFound if not object has given property with value
     */
    static Object findByKey(const Uri& propertyIRI, const PropertyValue& value);

    /**
     * Returns the only Object with property key set to value
     *
     * @param propertyIRI Internationalized Resource Identifier for the property
     * @param object resource to look for
     *
     * @throw DuplicateObject if more than one object have the same property value
     * @throw ObjectNotFound if not object has given property with value
     */
    static Object findByKey(const Uri& propertyIRI, const Object& object);

    /**
     * Dumps objects content to stream
     * @param recurse: if > 0, go down resource hierarchy by recurse level
     * @param indentLevel: How much layer of tabs to insert
     */
    std::ostream& printStream(std::ostream&, int recurse = 0, int indentLevel = 0) const;

    /**
     * Offered to interfaces
     */
    template<typename T> static std::list<T> findHelper(const Uri& iri) {
        Statement query;
        query.predicate.setIri(RDF_NS + "type");
        query.object.setIri(iri);
        const StatementList& statements = _factory->find(query);
        std::list<T> objList;
        for(const Statement& stmt : statements) {
            objList.push_back(T(_factory->createResourceFromStatement(stmt)));
        }
        return objList;
    }

    /**
     * Offered to interfaces
     * @throw InvalidIRI if propertyIRI is empty
     */
    template<typename T> std::list<T> getObjectListImpl(const Uri& propertyIRI) const {
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
    template<typename T> void setObjectListImpl(const Uri& propertyIRI, const std::list<T>& values) {
        addRdfTypeIfNeeded();
        Property p =_factory->createProperty(propertyIRI);
        _r.removeProperties(propertyIRI);
        for (const Object& obj: values) {
            p.setValue(obj._r);
            _r.addProperty(p);
        }
    }

    /**
     * Offered to interfaces
     * @throw InvalidIRI if propertyIRI is empty
     */
    template<cvt::RdfTypeEnum rdftype, typename T> std::list<T> getValueListImpl(const Uri& propertyIRI) const {
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

    /**
     * Offered to interfaces
     * @throw InvalidIRI if propertyIRI is empty
     */
    template<cvt::RdfTypeEnum rdftype, typename T> void setValueListImpl(const Uri& propertyIRI, const std::list<T>& values) {
        addRdfTypeIfNeeded();
        Property p = _factory->createProperty(propertyIRI);
        _r.removeProperties(propertyIRI);
        for (auto const & val: values) {
            p.setValue(PropertyValue().set<rdftype>(val));
            _r.addProperty(p);
        }
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

    Object(Resource r, const Uri& rdfTypeIRI = "");

    /**
     * Checks if underlying resource is of given type
     * If not add it to the types list
     */
    void addRdfTypeIfNeeded();

    /**
     * Shared constructor
     */
    void construct(const Uri& rdfTypeIRI);

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

#include <autordf/Object.h>
#include <stdexcept>
#include <set>

#include "autordf/Factory.h"

namespace autordf {

Factory *Object::_factory = nullptr;

void Object::setFactory(Factory *f) {
    _factory = f;
}

Object::Object(const std::string &iri) : _r(iri.empty() ? _factory->createBlankNodeResource() :_factory->createIRIResource(iri)) {
}

Object::Object(const Object& other) : _r(other._r) {
}

Object::Object(Resource r) : _r(r) {
}

const std::string& Object::iri() const {
    if ( _r.type() == NodeType::RESOURCE ) {
        return _r.name();
    } else {
        static const std::string& EMPTY="";
        return EMPTY;
    }
}

Object Object::getObject(const std::string &propertyIRI) const {
    return Object(_r.getProperty(propertyIRI).asResource());
}

std::shared_ptr<Object> Object::getOptionalObject(const std::string &propertyIRI) const {
    std::shared_ptr<Property> p(_r.getOptionalProperty(propertyIRI));
    if ( p ) {
        return std::shared_ptr<Object>(new Object(p->asResource()));
    } else {
        return nullptr;
    }
}

std::list<Object> Object::getObjectList(const std::string &propertyIRI) const {
    if ( propertyIRI.empty() ) {
        throw std::runtime_error("Calling Object::getObjectList() with empty IRI is forbidden");
    }
    std::list<Object> objList;
    const std::list<Property>& propList = _r.getPropertyValues(propertyIRI);
    for (const Property& prop: propList) {
        objList.push_back(Object(prop.asResource()));
    }
    return objList;
}

void Object::setObject(const std::string &propertyIRI, const Object &obj) {
    Property p = _factory->createProperty(propertyIRI);
    p.setValue(obj._r);
    _r.removeProperties(propertyIRI);
    _r.addProperty(p);
}

void Object::setObjectList(const std::string &propertyIRI, const std::list<Object> &values) {
    Property p =_factory->createProperty(propertyIRI);
    _r.removeProperties(propertyIRI);
    for (const Object& obj: values) {
        p.setValue(obj._r);
        _r.addProperty(p);
    }
}

PropertyValue Object::getPropertyValue(const std::string& propertyIRI) const {
    return _r.getProperty(propertyIRI).value();
}

std::shared_ptr<PropertyValue> Object::getOptionalPropertyValue(const std::string& propertyIRI) const {
    std::shared_ptr<Property> p(_r.getOptionalProperty(propertyIRI));
    if ( p ) {
        return std::make_shared<PropertyValue>(p->value());
    } else {
        return nullptr;
    }
}

std::list<PropertyValue> Object::getPropertyValueList(const std::string& propertyIRI) const {
    if ( propertyIRI.empty() ) {
        throw std::runtime_error("Calling Object::getPropertyValueList() with empty IRI is forbidden");
    }
    std::list<PropertyValue> valuesList;
    const std::list<Property>& propList = _r.getPropertyValues(propertyIRI);
    for (const Property& prop: propList) {
        valuesList.push_back(prop.value());
    }
    return valuesList;
}

void Object::setPropertyValue(const std::string& propertyIRI, const PropertyValue& val) {
    Property p = _factory->createProperty(propertyIRI);
    p.setValue(val);
    _r.removeProperties(propertyIRI);
    _r.addProperty(p);
}

void Object::setPropertyValueList(const std::string& propertyIRI, const std::list<PropertyValue>& values) {
    Property p = _factory->createProperty(propertyIRI);
    _r.removeProperties(propertyIRI);
    for (const PropertyValue& val: values) {
        p.setValue(val);
        _r.addProperty(p);
    }
}

void Object::remove() {
    _r.removeProperties("");
}

Object Object::clone(const std::string& iri) {
    const std::list<Property>& props = _r.getPropertyValues("");
    if ( !iri.empty() ) {
        return Object(_factory->createIRIResource(iri).setProperties(props));
    } else {
        return Object(_factory->createBlankNodeResource("").setProperties(props));
    }
}

std::list<Object> Object::findByType(const std::string& iri) {
    return findHelper<Object>(iri);
}

namespace {
void indentLine(std::ostream& os, int indent) {
    for (unsigned int i = 0; i < indent; ++i) {
        os << '\t';
    }
}
void newLine(std::ostream& os, int indent) {
    os << std::endl;
    indentLine(os, indent);
}
}

std::ostream& operator<<(std::ostream& os, const Object& obj) {
    obj.printStream(os);
    return os;
}

std::ostream& Object::printStream(std::ostream& os, int recurse, int indentLevel) const {
    indentLine(os, indentLevel);
    os << "\"name\": \"" << _r.name() <<'"';
    std::set<std::string> donePropsIRI;
    // Get all props
    const std::list<Property>& propsList = _r.getPropertyValues("");
    for ( auto propit = propsList.begin(); propit != propsList.end(); ++propit ) {
        if ( !donePropsIRI.count(propit->iri()) ) {
            if ( propit != propsList.begin() ) {
                os << ',';
            }
            newLine(os, indentLevel);
            donePropsIRI.insert(propit->iri());
            os << '"' << propit->iri() << "\": ";
            if ( (recurse > 0) && (propit->type() == NodeType::BLANK || propit->type() == NodeType::RESOURCE ) && propit->asResource().getPropertyValues("").size() ) {
                Object child(propit->asResource());
                os << '{' << std::endl;
                child.printStream(os, recurse - 1, indentLevel + 1);
                newLine(os, indentLevel);
                os << '}';
            } else {
                const std::list<PropertyValue>& values = getPropertyValueList(propit->iri());
                if ( values.size() > 1 ) {
                    os << '[';
                    newLine(os, indentLevel + 1);
                    for ( auto valit = values.begin(); valit != values.end(); ++valit) {
                        if ( valit != values.begin() ) {
                            os << ',';
                            newLine(os, indentLevel + 1);
                        }
                        os << '"' << *valit << '"';
                    }
                    newLine(os, indentLevel);
                    os << ']';
                } else {
                    os << '"' << getPropertyValue(propit->iri()) << '"';
                }
            }
        }
    }
    return os;
}

}

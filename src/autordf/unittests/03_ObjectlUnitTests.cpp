
#include <gtest/gtest.h>

#include <boost/filesystem.hpp>

#include "autordf/Factory.h"
#include "autordf/Object.h"

using namespace autordf;

void printRecurse(Object obj, int recurse) {
    std::cout << "Printing with recurse " << recurse << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "{" << std::endl;
    obj.printStream(std::cout, recurse, 1) << std::endl;
    std::cout << "}" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
}

TEST(_03_Object, Display) {
    Factory f;
    Object::setFactory(&f);

    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf", "http://xmlns.com/foaf/0.1/");

    std::list<Object> objs = Object::findByType("http://xmlns.com/foaf/0.1/Person");

    Object first = objs.front();

    printRecurse(first, 0);
    printRecurse(first, 1);
}

TEST(_03_Object, Copy) {
    Factory f;
    Object::setFactory(&f);

    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf", "http://xmlns.com/foaf/0.1/");

    std::list<Object> objs = Object::findByType("http://xmlns.com/foaf/0.1/Person");

    Object first = objs.front();

    ASSERT_EQ(3, Object::findByType("http://xmlns.com/foaf/0.1/Person").size());
    Object copy = first.clone("http://personcopy");
    ASSERT_EQ(4, Object::findByType("http://xmlns.com/foaf/0.1/Person").size());
    printRecurse(copy, 0);
}

TEST(_03_Object, Accessors) {
    Factory f;
    Object::setFactory(&f);

    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf", "http://xmlns.com/foaf/0.1/");

    std::list<Object> objs = Object::findByType("http://xmlns.com/foaf/0.1/Person");

    Object person = objs.front();

    ASSERT_NO_THROW(person.getObject("http://xmlns.com/foaf/0.1/account"));
    ASSERT_EQ(nullptr, person.getOptionalObject("http://xmlns.com/foaf/0.1/unexisting"));
    ASSERT_EQ(0, person.getObjectList("http://xmlns.com/foaf/0.1/unexisting").size());
    ASSERT_EQ(2, person.getObjectList("http://xmlns.com/foaf/0.1/knows").size());

    Object account = person.getObject("http://xmlns.com/foaf/0.1/account");
    ASSERT_EQ("Jimmy Wales", person.getPropertyValue("http://xmlns.com/foaf/0.1/name"));
    ASSERT_EQ(std::list<PropertyValue>({"Jimmy Wales"}), person.getPropertyValueList("http://xmlns.com/foaf/0.1/name"));

    // Trying to access a value as a ressource
    ASSERT_THROW(person.getObject("http://xmlns.com/foaf/0.1/name"), std::exception);

}
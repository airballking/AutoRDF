#ifndef AUTORDF_FACTORY_H
#define AUTORDF_FACTORY_H

#include <string>

#include <autordf/Model.h>
#include <autordf/Property.h>
#include <autordf/Resource.h>
#include <autordf/ResourceList.h>

namespace autordf {

class Factory : public Model {
public:
    /**
     * Create blank (anonymous) resource
     */
    Resource createBlankNodeResource(const std::string &bnodeid = "");

    /**
     * Created named resource (with IRI)
     */
    Resource createIRIResource(const std::string& iri);

    /**
     * Creates a property of type Literal
     */
    Property createProperty(const std::string& iri, NodeType type = NodeType::EMPTY);

    /**
     * Finds all ressources matching given type
     */
    ResourceList findByType(const std::string& typeIri);
};

}

#endif //AUTORDF_FACTORY_H
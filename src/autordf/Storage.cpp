#include "autordf/Storage.h"

#include "autordf/World.h"
#include "autordf/Exception.h"

namespace autordf {

Storage::Storage() {
    /* Default storage type, which is memory */
    _storage = librdf_new_storage(World().get(), NULL, NULL, NULL);
    if ( !_storage ) {
        throw InternalError("Failed to create RDF data storage");
    }
}

Storage::~Storage() {
    librdf_free_storage( _storage );
    _storage = 0;
}

}
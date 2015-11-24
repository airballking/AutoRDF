#include <autordf/ontology/Ontology.h>
#include "autordf/ontology/Klass.h"

namespace autordf {
namespace ontology {

std::set <std::shared_ptr<const Klass> > Klass::ancestors() const {
    std::set<std::shared_ptr<const Klass> > s;
    for (auto ancestor = _directAncestors.begin(); ancestor != _directAncestors.end(); ++ancestor) {
        s.insert(_ontology->classUri2Ptr().at(*ancestor));
    }
    return s;
}

std::set <std::shared_ptr<Klass> > Klass::ancestors() {
    std::set<std::shared_ptr<Klass> > s;
    for (auto ancestor = _directAncestors.begin(); ancestor != _directAncestors.end(); ++ancestor) {
        s.insert(_ontology->classUri2Ptr().at(*ancestor));
    }
    return s;
}

std::set<std::shared_ptr<const Klass> > Klass::getAllAncestors() const {
    std::set<std::shared_ptr<const Klass> > all;
    for ( auto ancestor = _directAncestors.begin(); ancestor != _directAncestors.end(); ++ancestor ) {
        all.insert(_ontology->classUri2Ptr().at(*ancestor));
        for ( std::shared_ptr<const Klass> more : _ontology->classUri2Ptr().at(*ancestor)->getAllAncestors() ) {
            all.insert(more);
        }
    }
    return all;
}
}
}



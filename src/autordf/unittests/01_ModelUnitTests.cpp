
#include <gtest/gtest.h>

#include <boost/filesystem.hpp>

#include "autordf/Model.h"

using namespace autordf;

TEST(_01_Model, Iterators) {
    Model ts;
    ts.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/example1.ttl", "http://test");
    StatementList allStatements = ts.find();
    ASSERT_EQ(4, std::distance(allStatements.begin(), allStatements.end()));
    const StatementList& allStatementsConst = ts.find();
    ASSERT_EQ(4, std::distance(allStatementsConst.begin(), allStatementsConst.end()));
}

TEST(_01_Model, SearchByPredicate) {
    Model ts;
    ts.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf", "http://test");

    Statement req;
    req.predicate.setIri("http://xmlns.com/foaf/0.1/name");
    const StatementList& stmtList = ts.find(req);
    ASSERT_EQ(3, stmtList.size());
}

TEST(_01_Model, SearchBySubject) {
    Model ts;
    ts.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf", "http://test");

    Statement req;
    req.subject.setIri("http://jimmycricket.com/me");
    const StatementList& stmtList = ts.find(req);
    ASSERT_EQ(2, stmtList.size());
}

TEST(_01_Model, SearchByObject) {
    Model ts;
    ts.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf", "http://test");

    Statement req;
    req.object.setLiteral("Jimmy");
    const StatementList& stmtList = ts.find(req);
    ASSERT_EQ(1, stmtList.size());
}


TEST(_01_Model, AddSaveEraseStatement) {
    Model ts;
    Statement st;
    st.subject.setIri("http://mydomain/me");
    st.predicate.setIri("http://mydomain/firstName");
    st.object.setLiteral("Fabien");

    ts.add(st);
    ts.saveToFile("/tmp/test1.ttl");

    Model read1;
    read1.loadFromFile("/tmp/test1.ttl", "http://test");
    const StatementList& stmtList = read1.find();
    ASSERT_EQ(1, stmtList.size());

    read1.remove(st);
    const StatementList& stmtList1 = read1.find();
    ASSERT_EQ(0, stmtList1.size());
}

TEST(DISABLED_01_Model, All) {
    Model ts;
    ts.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf", "http://test");
    const StatementList& stmtList = ts.find();
    for(const Statement& stmt: stmtList) {
        std::cout << stmt << std::endl;
    }
}


//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "MapIteratorTest"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

//Include user defined library
#include "../inc/ConcurrentHashMap.hpp"

#include <vector>

// ------------- Tests Follow --------------
//Name your test cases for what they test
BOOST_AUTO_TEST_CASE( iteratorOperations ) {
    const int NULL_VAL = -1;
    ConcurrentHashMap<unsigned, int, NULL_VAL, IdentityHash<unsigned> > ccMap(10000), ccMap2(10000);
    std::vector<int> v;
    for (int i = 0; i < 10000; i++) {
        v.push_back(i + 10000);
        v.push_back(i);
        ccMap.put((unsigned)i, i);
        ccMap.put((unsigned)(i + 10000), i + 10000);
    }

    int i = 0;
    for (ConcurrentHashMap<unsigned, int, NULL_VAL, IdentityHash<unsigned> >::iterator it = ccMap.begin(), ie = ccMap.end(); it != ie; ++it) {
        auto result = *it;
        BOOST_CHECK_EQUAL(result.first, (unsigned)v[i]);
        BOOST_CHECK_EQUAL(result.second, v[i]);
        i++;
    }
    auto it1 = ccMap.begin(), it2 = ccMap.begin(), it3 = ccMap.end(), it4 = ccMap.end();
    BOOST_CHECK_EQUAL(it1 == it2, true);
    BOOST_CHECK_EQUAL(it3 == it4, true);
    BOOST_CHECK_EQUAL(++it1 == ++it2, true);
    BOOST_CHECK_EQUAL(ccMap.end() == ++it3, true);
    BOOST_CHECK_EQUAL(ccMap.end() == ++it4, true);
    BOOST_CHECK_EQUAL(ccMap.begin() != ccMap.end(), true);
    BOOST_CHECK_EQUAL(ccMap2.begin() == ccMap2.end(), true);
}

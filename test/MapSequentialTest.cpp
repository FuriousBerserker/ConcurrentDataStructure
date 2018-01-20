//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "MapSequentialTest"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

//Include user defined library
#include "../inc/ConcurrentHashMap.hpp"

// ------------- Tests Follow --------------
//Name your test cases for what they test
BOOST_AUTO_TEST_CASE( sequentialPutAndGet )
{
    const int NULL_VAL = -1;
    ConcurrentHashMap<unsigned, int, NULL_VAL, IdentityHash<unsigned> > ccMap(10000);
    unsigned k1 = 3, k2 = 4, k3 = 10003, k4 = 10004;
    int v1 = k1, v2 = k2, v3 = k3, v4 = k4;
    BOOST_CHECK_EQUAL(ccMap.get(k1), NULL_VAL);
    ccMap.put(k1, v1);
    ccMap.put(k2, v2);
    ccMap.put(k3, v3);
    ccMap.put(k4, v4);
    BOOST_CHECK_EQUAL(ccMap.get(k1), v1); 
    BOOST_CHECK_EQUAL(ccMap.get(k2), v2); 
    BOOST_CHECK_EQUAL(ccMap.get(k3), v3); 
    BOOST_CHECK_EQUAL(ccMap.get(k4), v4); 
    ccMap.put(k4, v4 + 1);
    BOOST_CHECK_EQUAL(ccMap.get(k4), v4 + 1);
    ConcurrentHashMap<unsigned, int, NULL_VAL> ccMap2(10000);
    BOOST_CHECK_EQUAL(ccMap2.get(k1), NULL_VAL);
    ccMap2.put(k1, v1);
    ccMap2.put(k2, v2);
    ccMap2.put(k3, v3);
    ccMap2.put(k4, v4);
    BOOST_CHECK_EQUAL(ccMap2.get(k1), v1); 
    BOOST_CHECK_EQUAL(ccMap2.get(k2), v2); 
    BOOST_CHECK_EQUAL(ccMap2.get(k3), v3); 
    BOOST_CHECK_EQUAL(ccMap2.get(k4), v4); 
    ccMap2.put(k4, v4 + 1);
    BOOST_CHECK_EQUAL(ccMap2.get(k4), v4 + 1);

}

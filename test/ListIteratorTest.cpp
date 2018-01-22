//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "ListIteratorTest"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

//Include user defined library
#include "../inc/ConcurrentList.hpp"
// ------------- Tests Follow --------------
//Name your test cases for what they test
BOOST_AUTO_TEST_CASE( iteratorOperations ) {
   ConcurrentList<int> ll;
   for (int i = 0; i < 100; i++) {
    ll.insert(i);
   }
   int v = 99;
   for (auto it = ll.begin(), ie = ll.end(); it != ie; ++it) {
    int value = *it;
    BOOST_CHECK_EQUAL(value, v);
    v--;
   }
    auto it1 = ll.begin(), it2 = ll.begin(), it3 = ll.end(), it4 = ll.end();
    BOOST_CHECK_EQUAL(it1 == it2, true);
    BOOST_CHECK_EQUAL(it3 == it4, true);
    BOOST_CHECK_EQUAL(++it1 == ++it2, true);
    BOOST_CHECK_EQUAL(ll.end() == ++it3, true);
    BOOST_CHECK_EQUAL(ll.end() == ++it4, true);
    BOOST_CHECK_EQUAL(ll.begin() != ll.end(), true);
    ConcurrentList<int> ll2;
    BOOST_CHECK_EQUAL(ll2.begin() == ll2.end(), true);
    ll2.insert(2);
    BOOST_CHECK_EQUAL(ll2.begin() != ll2.end(), true);
    BOOST_CHECK_EQUAL((++ll2.begin()) == ll2.end(), true);
}

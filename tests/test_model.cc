/*
 * A sample test case which can be used as a template.
 */
#include <iostream>
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include "../src/model.h"

class TestModel : public CppUnit::TestCase {

  CPPUNIT_TEST_SUITE( TestModel );

  CPPUNIT_TEST( testAddChangeTime );
  CPPUNIT_TEST( testDebugConstructor );

  CPPUNIT_TEST_SUITE_END();

 public:
  void testAddChangeTime() {
    Model model = Model();
    std::vector<size_t> v1, v2, v3; 

    // Check basic adding first time;
    CPPUNIT_ASSERT( model.addChangeTime(1) == 0 );
    CPPUNIT_ASSERT( model.change_times_.size() == 1 );
    CPPUNIT_ASSERT( model.change_times_.at(0) == 1 ); 
    CPPUNIT_ASSERT( model.sample_sizes_list_.size() == 1 );
    CPPUNIT_ASSERT( model.population_sizes_list_.size() == 1 );
    model.sample_sizes_list_[0] = &v1;

    // Check adding a time at the end;
    CPPUNIT_ASSERT_EQUAL( (size_t)1, model.addChangeTime(3) );
    CPPUNIT_ASSERT( model.change_times_.size() == 2 );
    CPPUNIT_ASSERT( model.change_times_.at(1) == 3 ); 
    CPPUNIT_ASSERT( model.sample_sizes_list_[0] == &v1 ); 
    CPPUNIT_ASSERT( model.sample_sizes_list_[1] == NULL ); 
    model.sample_sizes_list_[1] = &v3;

    // Check adding a time in the middle;
    CPPUNIT_ASSERT_EQUAL( (size_t)1, model.addChangeTime(2) );
    CPPUNIT_ASSERT( model.change_times_.size() == 3 );
    CPPUNIT_ASSERT( model.change_times_.at(0) == 1 ); 
    CPPUNIT_ASSERT( model.change_times_.at(1) == 2 ); 
    CPPUNIT_ASSERT( model.change_times_.at(2) == 3 ); 
    CPPUNIT_ASSERT( model.sample_sizes_list_[0] == &v1 ); 
    CPPUNIT_ASSERT( model.sample_sizes_list_[1] == NULL ); 
    CPPUNIT_ASSERT( model.sample_sizes_list_[2] == &v3 ); 
    model.sample_sizes_list_[1] = &v2;

    // Check adding a time in the beginning;
    CPPUNIT_ASSERT_EQUAL( (size_t)0, model.addChangeTime(0) );
    CPPUNIT_ASSERT( model.change_times_.size() == 4 );
    CPPUNIT_ASSERT( model.change_times_.at(0) == 0 ); 
    CPPUNIT_ASSERT( model.change_times_.at(1) == 1 );
    CPPUNIT_ASSERT( model.sample_sizes_list_[0] == NULL ); 
    CPPUNIT_ASSERT( model.sample_sizes_list_[1] == &v1 ); 
    CPPUNIT_ASSERT( model.sample_sizes_list_[2] == &v2 ); 
    CPPUNIT_ASSERT( model.sample_sizes_list_[3] == &v3 ); 

    // Check that we don't add a time twice 
    CPPUNIT_ASSERT_EQUAL( (size_t)0, model.addChangeTime(0) );
    CPPUNIT_ASSERT_EQUAL( (size_t)1, model.addChangeTime(1) );
    CPPUNIT_ASSERT_EQUAL( (size_t)2, model.addChangeTime(2) );
    CPPUNIT_ASSERT_EQUAL( (size_t)3, model.addChangeTime(3) );
    CPPUNIT_ASSERT( model.change_times_.size() == 4 );
    CPPUNIT_ASSERT( model.sample_sizes_list_.size() == 4 );
    CPPUNIT_ASSERT( model.population_sizes_list_.size() == 4 );
  }

  void testDebugConstructor() {
    Model model = Model(7);
    //CPPUNIT_ASSERT( model.sample_size() == 7 );
  }

  void testGettersAndSetters() {
    //model.set_exact_window_length(100);
    // CPPUNIT_ASSERT( model.exact_window_length() == 100 );
  }

  
};

//Uncomment this to activate the test
CPPUNIT_TEST_SUITE_REGISTRATION( TestModel );

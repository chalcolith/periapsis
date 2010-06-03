// Wed Jun  2 17:14:24 2010

#include "stdafx.h"

#ifdef WIN32
#pragma warning (disable : 4273)
#endif

#ifdef WIN32
#define TEST_DLL_API __declspec(dllexport)
#else
#define TEST_DLL_API 
#endif

#include "data\test_array.hpp"
#include "data\test_dictionary.hpp"
#include "data\test_exception.hpp"
#include "data\test_fd_stream.hpp"
#include "data\test_queue.hpp"
#include "math\test_transform.hpp"


#include <string>

extern "C" {


TEST_DLL_API void test_math_points_test_points();
			
void test_math_points_test_points() {
    test::math::points *ptr = new test::math::points();
    ptr->test_points();
    delete ptr;
}

TEST_DLL_API void test_math_points_test_rot();
			
void test_math_points_test_rot() {
    test::math::points *ptr = new test::math::points();
    ptr->test_rot();
    delete ptr;
}

TEST_DLL_API void test_data_dict_remove_test_remove();
			
void test_data_dict_remove_test_remove() {
    test::data::dict_remove *ptr = new test::data::dict_remove();
    ptr->test_remove();
    delete ptr;
}

TEST_DLL_API void test_data_floats_constant_test_rw();
			
void test_data_floats_constant_test_rw() {
    test::data::floats_constant *ptr = new test::data::floats_constant();
    ptr->test_rw();
    delete ptr;
}

TEST_DLL_API void test_data_dict_mixed2_test_size();
			
void test_data_dict_mixed2_test_size() {
    test::data::dict_mixed2 *ptr = new test::data::dict_mixed2();
    ptr->test_size();
    delete ptr;
}

TEST_DLL_API void test_data_dict_mixed2_test_order();
			
void test_data_dict_mixed2_test_order() {
    test::data::dict_mixed2 *ptr = new test::data::dict_mixed2();
    ptr->test_order();
    delete ptr;
}

TEST_DLL_API void test_data_array_simple_test_insert();
			
void test_data_array_simple_test_insert() {
    test::data::array_simple *ptr = new test::data::array_simple();
    ptr->test_insert();
    delete ptr;
}

TEST_DLL_API void test_data_array_simple_test_remove();
			
void test_data_array_simple_test_remove() {
    test::data::array_simple *ptr = new test::data::array_simple();
    ptr->test_remove();
    delete ptr;
}

TEST_DLL_API void test_data_array_simple_test_iter_traverse();
			
void test_data_array_simple_test_iter_traverse() {
    test::data::array_simple *ptr = new test::data::array_simple();
    ptr->test_iter_traverse();
    delete ptr;
}

TEST_DLL_API void test_data_array_simple_test_iter_insert();
			
void test_data_array_simple_test_iter_insert() {
    test::data::array_simple *ptr = new test::data::array_simple();
    ptr->test_iter_insert();
    delete ptr;
}

TEST_DLL_API void test_data_dict_left_test_size();
			
void test_data_dict_left_test_size() {
    test::data::dict_left *ptr = new test::data::dict_left();
    ptr->test_size();
    delete ptr;
}

TEST_DLL_API void test_data_dict_left_test_order();
			
void test_data_dict_left_test_order() {
    test::data::dict_left *ptr = new test::data::dict_left();
    ptr->test_order();
    delete ptr;
}

TEST_DLL_API void test_data_dict_mixed_test_size();
			
void test_data_dict_mixed_test_size() {
    test::data::dict_mixed *ptr = new test::data::dict_mixed();
    ptr->test_size();
    delete ptr;
}

TEST_DLL_API void test_data_dict_mixed_test_order();
			
void test_data_dict_mixed_test_order() {
    test::data::dict_mixed *ptr = new test::data::dict_mixed();
    ptr->test_order();
    delete ptr;
}

TEST_DLL_API void test_data_simple_queue_basic_test_001();
			
void test_data_simple_queue_basic_test_001() {
    test::data::simple_queue_basic *ptr = new test::data::simple_queue_basic();
    ptr->test_001();
    delete ptr;
}

TEST_DLL_API void test_data_simple_queue_basic_test_002();
			
void test_data_simple_queue_basic_test_002() {
    test::data::simple_queue_basic *ptr = new test::data::simple_queue_basic();
    ptr->test_002();
    delete ptr;
}

TEST_DLL_API void test_data_floats_random_test_rw();
			
void test_data_floats_random_test_rw() {
    test::data::floats_random *ptr = new test::data::floats_random();
    ptr->test_rw();
    delete ptr;
}

TEST_DLL_API void test_data_exception_basic_test_throw();
			
void test_data_exception_basic_test_throw() {
    test::data::exception_basic *ptr = new test::data::exception_basic();
    ptr->test_throw();
    delete ptr;
}

TEST_DLL_API void test_data_dict_balanced_test_size();
			
void test_data_dict_balanced_test_size() {
    test::data::dict_balanced *ptr = new test::data::dict_balanced();
    ptr->test_size();
    delete ptr;
}

TEST_DLL_API void test_data_dict_balanced_test_order();
			
void test_data_dict_balanced_test_order() {
    test::data::dict_balanced *ptr = new test::data::dict_balanced();
    ptr->test_order();
    delete ptr;
}

TEST_DLL_API void test_data_dict_right_test_size();
			
void test_data_dict_right_test_size() {
    test::data::dict_right *ptr = new test::data::dict_right();
    ptr->test_size();
    delete ptr;
}

TEST_DLL_API void test_data_dict_right_test_order();
			
void test_data_dict_right_test_order() {
    test::data::dict_right *ptr = new test::data::dict_right();
    ptr->test_order();
    delete ptr;
}

TEST_DLL_API void test_data_array_object_test_insert();
			
void test_data_array_object_test_insert() {
    test::data::array_object *ptr = new test::data::array_object();
    ptr->test_insert();
    delete ptr;
}

TEST_DLL_API void test_data_array_object_test_remove();
			
void test_data_array_object_test_remove() {
    test::data::array_object *ptr = new test::data::array_object();
    ptr->test_remove();
    delete ptr;
}

TEST_DLL_API void test_data_array_object_test_iter_traverse();
			
void test_data_array_object_test_iter_traverse() {
    test::data::array_object *ptr = new test::data::array_object();
    ptr->test_iter_traverse();
    delete ptr;
}


struct {
    char *name;
    void (*test)(void);
} test_functions[] = {
    { "test::math::points::test_points", test_math_points_test_points },
    { "test::math::points::test_rot", test_math_points_test_rot },
    { "test::data::dict_remove::test_remove", test_data_dict_remove_test_remove },
    { "test::data::floats_constant::test_rw", test_data_floats_constant_test_rw },
    { "test::data::dict_mixed2::test_size", test_data_dict_mixed2_test_size },
    { "test::data::dict_mixed2::test_order", test_data_dict_mixed2_test_order },
    { "test::data::array_simple::test_insert", test_data_array_simple_test_insert },
    { "test::data::array_simple::test_remove", test_data_array_simple_test_remove },
    { "test::data::array_simple::test_iter_traverse", test_data_array_simple_test_iter_traverse },
    { "test::data::array_simple::test_iter_insert", test_data_array_simple_test_iter_insert },
    { "test::data::dict_left::test_size", test_data_dict_left_test_size },
    { "test::data::dict_left::test_order", test_data_dict_left_test_order },
    { "test::data::dict_mixed::test_size", test_data_dict_mixed_test_size },
    { "test::data::dict_mixed::test_order", test_data_dict_mixed_test_order },
    { "test::data::simple_queue_basic::test_001", test_data_simple_queue_basic_test_001 },
    { "test::data::simple_queue_basic::test_002", test_data_simple_queue_basic_test_002 },
    { "test::data::floats_random::test_rw", test_data_floats_random_test_rw },
    { "test::data::exception_basic::test_throw", test_data_exception_basic_test_throw },
    { "test::data::dict_balanced::test_size", test_data_dict_balanced_test_size },
    { "test::data::dict_balanced::test_order", test_data_dict_balanced_test_order },
    { "test::data::dict_right::test_size", test_data_dict_right_test_size },
    { "test::data::dict_right::test_order", test_data_dict_right_test_order },
    { "test::data::array_object::test_insert", test_data_array_object_test_insert },
    { "test::data::array_object::test_remove", test_data_array_object_test_remove },
    { "test::data::array_object::test_iter_traverse", test_data_array_object_test_iter_traverse },

    { 0, 0 }
};

#if defined(WIN32)
    TEST_DLL_API void *get_test_functions() {
        return test_functions;
    }
#endif

} // extern "C"

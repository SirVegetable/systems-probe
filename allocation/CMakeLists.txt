
find_package(Boost CONFIG REQUIRED)


add_executable(tx_alignment_test tx_alignment_test.cpp)
target_include_directories(tx_alignment_test PRIVATE ${Boost_INCLUDE_DIRS})
target_compile_features(tx_alignment_test PRIVATE cxx_std_23)
target_compile_options(tx_alignment_test PRIVATE -Wall -Wextra -Werror)


add_executable(message_queue_tests message_queue_tests.cpp)
target_include_directories(message_queue_tests PRIVATE ${Boost_INCLUDE_DIRS})
target_compile_features(message_queue_tests PRIVATE cxx_std_23)
target_compile_options(message_queue_tests PRIVATE -Wall -Wextra -Werror)


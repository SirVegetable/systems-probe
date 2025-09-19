
find_package(Boost CONFIG REQUIRED)

add_executable(unnamed_mem_block unnamed_mem_block.cpp)
target_include_directories(unnamed_mem_block PRIVATE ${Boost_INCLUDE_DIRS})
target_compile_features(unnamed_mem_block PRIVATE cxx_std_23)
target_compile_options(unnamed_mem_block PRIVATE -Wall -Wextra -Werror)


add_executable(named_mem_block named_mem_block.cpp)
target_include_directories(named_mem_block PRIVATE ${Boost_INCLUDE_DIRS})
target_compile_features(named_mem_block PRIVATE cxx_std_23)
target_compile_options(named_mem_block PRIVATE -Wall -Wextra -Werror)


add_executable(offset_ptr_basics offset_ptr_basics.cpp)
target_include_directories(offset_ptr_basics PRIVATE ${Boost_INCLUDE_DIRS})
target_compile_features(offset_ptr_basics PRIVATE cxx_std_23)
target_compile_options(offset_ptr_basics PRIVATE -Wall -Wextra -Werror)
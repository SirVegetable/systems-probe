
find_package(Boost CONFIG REQUIRED)

add_executable(unnamed_mem_block unnamed_mem_block.cpp)
target_include_directories(unnamed_mem_block PRIVATE ${Boost_INCLUDE_DIRS})
target_compile_features(unnamed_mem_block PRIVATE cxx_std_23)
target_compile_options(unnamed_mem_block PRIVATE -Wall -Wextra -Werror)

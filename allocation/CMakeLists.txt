

add_executable(test test.cpp)
target_compile_features(test PRIVATE cxx_std_23)
target_compile_options(test PRIVATE -Wall -Wextra -Werror)
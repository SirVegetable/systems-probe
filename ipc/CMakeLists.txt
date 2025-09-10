

add_executable(increment_counter increment_counter.cpp)
target_compile_features(increment_counter PRIVATE cxx_std_23)
target_compile_options(increment_counter PRIVATE -Wall -Wextra -Werror)
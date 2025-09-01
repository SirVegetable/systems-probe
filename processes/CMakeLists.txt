

add_executable(basic_pids_runner basic_pids.cpp)
target_compile_features(basic_pids_runner PRIVATE cxx_std_23)
target_compile_options(basic_pids_runner PRIVATE -Wall -Wextra -Werror)
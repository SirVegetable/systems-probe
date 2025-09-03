

add_executable(basic_pids_runner basic_pids.cpp)
target_compile_features(basic_pids_runner PRIVATE cxx_std_23)
target_compile_options(basic_pids_runner PRIVATE -Wall -Wextra -Werror)


add_executable(child_changes_globals_runner child_changes_globals.cpp)
target_compile_features(child_changes_globals_runner PRIVATE cxx_std_23)
target_compile_options(child_changes_globals_runner PRIVATE -Wall -Wextra -Werror)


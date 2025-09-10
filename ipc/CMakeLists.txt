

add_executable(increment_counter increment_counter.cpp)
target_compile_features(increment_counter PRIVATE cxx_std_23)
target_compile_options(increment_counter PRIVATE -Wall -Wextra -Werror)


add_executable(increment_shared_semaphore increment_shared_semaphore.cpp)
target_compile_features(increment_shared_semaphore PRIVATE cxx_std_23)
target_compile_options(increment_shared_semaphore PRIVATE -Wall -Wextra -Werror)
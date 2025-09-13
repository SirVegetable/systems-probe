

add_executable(increment_counter increment_counter.cpp)
target_compile_features(increment_counter PRIVATE cxx_std_23)
target_compile_options(increment_counter PRIVATE -Wall -Wextra -Werror)

add_executable(increment_shared_semaphore increment_shared_semaphore.cpp)
target_compile_features(increment_shared_semaphore PRIVATE cxx_std_23)
target_compile_options(increment_shared_semaphore PRIVATE -Wall -Wextra -Werror)

add_executable(mapped_sizing mapped_sizing.cpp)
target_compile_features(mapped_sizing PRIVATE cxx_std_23)
target_compile_options(mapped_sizing PRIVATE -Wall -Wextra -Werror)

add_executable(growing_file growing_file.cpp)
target_compile_features(growing_file PRIVATE cxx_std_23)
target_compile_options(growing_file PRIVATE -Wall -Wextra -Werror)

add_executable(shm_create shm_create.cpp)
target_compile_features(shm_create PRIVATE cxx_std_23)
target_compile_options(shm_create PRIVATE -Wall -Wextra -Werror)

add_executable(shm_delete shm_delete.cpp)
target_compile_features(shm_delete PRIVATE cxx_std_23)
target_compile_options(shm_delete PRIVATE -Wall -Wextra -Werror)

add_executable(shm_simple_reader shm_simple_reader.cpp)
target_compile_features(shm_simple_reader PRIVATE cxx_std_23)
target_compile_options(shm_simple_reader PRIVATE -Wall -Wextra -Werror)

add_executable(shm_simple_writer shm_simple_writer.cpp)
target_compile_features(shm_simple_writer PRIVATE cxx_std_23)
target_compile_options(shm_simple_writer PRIVATE -Wall -Wextra -Werror)
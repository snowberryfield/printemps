# CPU_ARCH controls architecture-specific compiler flags in Release builds.
#   "native"        (default) — -mcpu=native (arm64/aarch64) or -march=native -mtune=native (x86_64)
#   "none"          — no architecture flags
#   any other value — passed directly as -mcpu=<value> (arm64/aarch64) or -march=<value> -mtune=generic (x86_64)
set(CPU_ARCH "native" CACHE STRING
    "Target CPU architecture: 'native' (default), 'none', or a value such as x86-64-v2 / armv8-a")

if(NOT MSVC)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        if(CPU_ARCH STREQUAL "none")
            # no architecture flags
        elseif(CPU_ARCH STREQUAL "native")
            if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
                add_compile_options(-mcpu=native)
            else()
                add_compile_options(-march=native -mtune=native)
            endif()
        else()
            if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
                add_compile_options(-mcpu=${CPU_ARCH})
            else()
                add_compile_options(-march=${CPU_ARCH} -mtune=generic)
            endif()
        endif()
    endif()
endif()

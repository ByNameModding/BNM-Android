cmake_minimum_required(VERSION 3.8...3.26)


project(BNM)

# Собирать примеры BNM
# Build BNM examples
set(BNM_BUILD_EXAMPLES OFF)

# Добавить зависимость на liblog.so
# Add dependents to liblog.so
set(BNM_LINK_LOG OFF)

set(MY_DIR ${CMAKE_CURRENT_SOURCE_DIR})


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PUBLIC_INCLUDE_DIRS
    include
    external/include
)

set(INCLUDE_DIRS
    include
    external/include
    external
    external/utf8
    src/BNM_data
)

add_library(
        BNM
        STATIC
        src/Class.cpp
        src/ClassesManagement.cpp
        src/Coroutine.cpp
        src/Delegates.cpp
        src/EventBase.cpp
        src/FieldBase.cpp
        src/Hooks.cpp
        src/Image.cpp
        src/Internals.cpp
        src/Loading.cpp
        src/MethodBase.cpp
        src/MonoStructures.cpp
        src/PropertyBase.cpp
        src/UnityStructures.cpp
        src/Utils.cpp
)

target_include_directories(
        BNM
        PUBLIC
        ${INCLUDE_DIRS}
)

if (${BNM_LINK_LOG})
    target_link_libraries(
            BNM
            PUBLIC
            log
    )
endif ()


set_property(TARGET BNM PROPERTY BNM_INCLUDE_DIRECTORIES ${PUBLIC_INCLUDE_DIRS})

if (${BNM_BUILD_EXAMPLES})
    add_library(BNM_Examples
            SHARED
            examples/Main.cpp
            examples/01_Basics.cpp
            examples/02_OtherStructures.cpp
            examples/04_CompileTimeClasses.cpp
            examples/05_ClassesManagement.cpp
            examples/03_Generic.cpp
    )
    add_library(BNM_Old_Examples
            SHARED
            examples/OldExamples.cpp
    )

    target_include_directories(
            BNM_Examples
            PUBLIC
            ${PUBLIC_INCLUDE_DIRS}
    )

    target_include_directories(
            BNM_Old_Examples
            PUBLIC
            ${PUBLIC_INCLUDE_DIRS}
    )

    target_link_libraries(
            BNM_Examples
            PUBLIC
            BNM
            log
    )
    target_link_libraries(
            BNM_Old_Examples
            PUBLIC
            BNM
            log
    )

endif ()


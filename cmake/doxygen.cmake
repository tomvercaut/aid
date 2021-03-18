find_package(Doxygen REQUIRED dot mscgen dia)

set(DOXYGEN_PROJECT_BRIEF "Library with practical helper classes and functions.")
set(DOXYGEN_DOXYGEN_RECURSIVE YES)
set(DOXYGEN_EXTRACT_ALL YES)

doxygen_add_docs(aid_docs
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/tests

)
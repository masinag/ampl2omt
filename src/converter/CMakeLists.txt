add_library(smtlib_converter
        definitions_mapper.cpp
        smtlib_converter.cpp
        smtlib_term_converter.cpp
)

target_link_libraries(smtlib_converter PUBLIC mp)
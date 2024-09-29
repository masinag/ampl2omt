add_subdirectory(converter)

add_executable(ampl2omt ampl2omt.cpp)
target_link_libraries(ampl2omt PRIVATE smtlib_converter)
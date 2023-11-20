# could be added to file above
# but not yet required
# lets leave it here
message(STATUS "thor_scsi::core gtpsa cpp include dir ${gtpsa_cpp_INCLUDE_DIR}")

add_executable(test_math_comb
        test_math_comb.cc
)
target_include_directories(test_math_comb
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../>"
)
target_link_libraries(test_math_comb
        thor_scsi_core
        ${Boost_PRG_EXEC_MONITOR_LIBRARY}
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

add_test(math_comb test_math_comb)

add_executable(test_transform
        test_transform.cc
)


target_include_directories(test_transform
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../>"
        "$<BUILD_INTERFACE:${gtpsa_cpp_INCLUDE_DIR}>"
        "$<BUILD_INTERFACE:${gtpsa_mad_ng_INCLUDE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(test_transform
        ${Boost_PRG_EXEC_MONITOR_LIBRARY}
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

add_test(transform test_transform)


add_executable(test_two_dimensional_multipoles
  test_two_dimensional_multipoles.cc
  multipoles.cc
  # multipoles_extra.cc
  math_comb.cc
)
add_test(two_dimensional_multipoles test_two_dimensional_multipoles)

target_include_directories(test_two_dimensional_multipoles
    PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../>"
    "$<BUILD_INTERFACE:${gtpsa_cpp_INCLUDE_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_link_libraries(test_two_dimensional_multipoles
        gtpsa::gtpsa-c++
        gtpsa::gtpsa
        ${Boost_PRG_EXEC_MONITOR_LIBRARY}
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

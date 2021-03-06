
add_library( cps_egenesis_none
   egenesis_none.cpp
   include/cps/egenesis/egenesis.hpp
   )

target_link_libraries( cps_egenesis_none cps_chain cps_native_contract fc )
target_include_directories( cps_egenesis_none
   PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

add_executable( embed_genesis
   embed_genesis.cpp
   )

target_link_libraries( embed_genesis cps_chain cps_egenesis_none fc )

set( embed_genesis_args
   -t "${CMAKE_CURRENT_SOURCE_DIR}/egenesis_brief.cpp.tmpl---${CMAKE_CURRENT_BINARY_DIR}/egenesis_brief.cpp"
   -t "${CMAKE_CURRENT_SOURCE_DIR}/egenesis_full.cpp.tmpl---${CMAKE_CURRENT_BINARY_DIR}/egenesis_full.cpp"
   )

MESSAGE( STATUS "egenesis: " ${CPS_EGENESIS_JSON} )

if( CPS_EGENESIS_JSON )
   list( APPEND embed_genesis_args --genesis-json "${CPS_EGENESIS_JSON}" )
endif( CPS_EGENESIS_JSON )

MESSAGE( STATUS "embed_genesis_args: " ${embed_genesis_args} )

add_custom_command(
   OUTPUT
      "${CMAKE_CURRENT_BINARY_DIR}/egenesis_brief.cpp"
      "${CMAKE_CURRENT_BINARY_DIR}/egenesis_full.cpp"
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
   COMMAND embed_genesis ${embed_genesis_args}
   DEPENDS
      "${CPS_EGENESIS_JSON}"
      "${CMAKE_CURRENT_SOURCE_DIR}/egenesis_brief.cpp.tmpl"
      "${CMAKE_CURRENT_SOURCE_DIR}/egenesis_full.cpp.tmpl"
      embed_genesis )

add_library( cps_egenesis_brief "${CMAKE_CURRENT_BINARY_DIR}/egenesis_brief.cpp" include/cps/egenesis/egenesis.hpp )
add_library( cps_egenesis_full  "${CMAKE_CURRENT_BINARY_DIR}/egenesis_full.cpp" include/cps/egenesis/egenesis.hpp )

target_link_libraries( cps_egenesis_brief cps_chain cps_native_contract fc )
target_link_libraries( cps_egenesis_full cps_chain cps_native_contract fc )

target_include_directories( cps_egenesis_brief
   PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
target_include_directories( cps_egenesis_full
   PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

INSTALL( TARGETS
   embed_genesis cps_egenesis_none cps_egenesis_brief cps_egenesis_full

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)

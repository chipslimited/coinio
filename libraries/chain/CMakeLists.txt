file(GLOB HEADERS "include/cps/chain/*.hpp")

## SORT .cpp by most likely to change / break compile
add_library( cps_chain
             wast_to_wasm.cpp
             chain_controller.cpp
             wasm_interface.cpp
             block_schedule.cpp

             fork_database.cpp

             transaction.cpp
             block.cpp

             get_config.cpp

             block_log.cpp
             blockchain_configuration.cpp

             types.cpp
             chain_administration_interface.cpp
             message_handling_contexts.cpp
             producer_objects.cpp
             staked_balance_objects.cpp

             ${HEADERS}
           )

target_link_libraries( cps_chain fc chainbase cps_types Logging IR WAST WASM Runtime )
target_include_directories( cps_chain
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_BINARY_DIR}/include"
                                   "${CMAKE_CURRENT_SOURCE_DIR}/../wasm-jit/Include"
                            )

if(MSVC)
  set_source_files_properties( db_init.cpp db_block.cpp database.cpp block_log.cpp PROPERTIES COMPILE_FLAGS "/bigobj" )
endif(MSVC)

INSTALL( TARGETS
   cps_chain

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
INSTALL( FILES ${HEADERS} DESTINATION "include/cps/chain" )

file(GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")
PROTOBUF_GENERATE_CPP (ProtoSources_ ProtoHeaders ${ProtoFiles})
add_library(protocols STATIC ${ProtoSources_})
target_link_libraries(protocols ${PROTOBUF_LIBRARY})
#set(ProtoSources ${ProtoSources_}
#    CACHE INTERNAL "Sources generated by protobuf")
set(ProtoIncludeDir ${CMAKE_CURRENT_BINARY_DIR}
    CACHE INTERNAL "Files generated by protobuf")

FUNCTION (GENERATE_PROTO_C ProtoFile SRC HDR)
    add_custom_command(OUTPUT ${SRC} ${HDR}
        COMMAND "protoc-c"
        ARGS -I${CMAKE_CURRENT_SOURCE_DIR} ${ProtoFile} --c_out=${CMAKE_CURRENT_BINARY_DIR})
ENDFUNCTION (GENERATE_PROTO_C)

FUNCTION (PROTOBUF_GENERATE_C SRC HDR)
    set(srcLoc "")
    set(hdrLoc "")
    foreach (f ${ARGN})
        GENERATE_PROTO_C (${f} "${f}.pb-c.c" "${f}.pb-c.h")
        get_filename_component(tmpFn ${f} NAME_WE)
        LIST(APPEND srcLoc "${CMAKE_CURRENT_BINARY_DIR}/${tmpFn}.pb-c.c")
        LIST(APPEND hdrLoc "${CMAKE_CURRENT_BINARY_DIR}/${tmpFn}.pb-c.h")
    ENDFOREACH(f)
    set(${SRC} "${srcLoc}" PARENT_SCOPE)
    set(${HDR} "${hdrLoc}" PARENT_SCOPE)
ENDFUNCTION (PROTOBUF_GENERATE_C)

PROTOBUF_GENERATE_C (ProtoSrcC ProtoHdrC ${ProtoFiles})
#MESSAGE(STATUS "Proto: " ${ProtoFiles})
#MESSAGE(STATUS "Protoc: " ${ProtoSrcC})

add_library(cprotocols STATIC ${ProtoSrcC})
target_link_libraries(cprotocols ${PROTOBUF_LIBRARY})
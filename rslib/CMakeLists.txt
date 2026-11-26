cmake_minimum_required(VERSION 3.16)
project(RSLib VERSION 0.6.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)

set(RSLIB_SRC
  src/both/action.c
  src/both/asset.c
  src/both/base37.c
  src/both/buf.c
  src/both/buf_bits.c
  src/both/buf_get.c
  src/both/buf_put.c
  src/both/build.c
  src/both/cam.c
  src/both/build_caps.c
  src/both/cache.c
  src/both/cache_js5.c
  src/both/cache_keys.c
  src/both/cache_read.c
  src/both/chat.c
  src/both/codec.c
  src/both/codec_bridge.c
  src/both/codec_delegates.c
  src/both/codec_payload.c
  src/both/codec_game.c
  src/both/compress.c
  src/both/config.c
  src/both/content.c
  src/both/crc.c
  src/both/dat.c
  src/both/err.c
  src/both/era_body.c
  src/both/hash.c
  src/both/isaac.c
  src/both/jag.c
  src/both/js5.c
  src/both/login_ext.c
  src/both/pack.c
  src/both/path.c
  src/both/packets.c
  src/both/packet_wire_report.c
  src/both/packets_names.c
  src/both/sha256.c
  src/both/rsa.c
  src/both/rsc_game.c
  src/both/sock.c
  src/both/store.c
  src/both/sync.c
  src/both/time.c
  src/both/whirlpool.c
  src/both/worldlist.c
  src/both/xtea.c
  src/both/zonebuf.c
  src/proto/frame.c
  src/proto/js5.c
  src/proto/js5_handshake.c
  src/proto/login_facts.c
  src/proto/js5_login.c
  src/proto/js5_lobby.c
  src/proto/js5_update.c
  src/proto/proto.c
  src/proto/rs2.c
  src/proto/rs2_handshake.c
  src/proto/rs2_login.c
  src/proto/rs2_ondemand.c
  src/proto/rsc.c
  src/proto/session.c
  src/client/client.c
  src/client/connect.c
  src/client/handshake.c
  src/client/login.c
  src/client/packets.c
  src/client/emit.c
  src/client/action_send.c
  src/client/packets/115.c
  src/client/packets/159.c
  src/client/packets/177.c
  src/client/packets/254.c
  src/client/packets/317.c
  src/client/packets/377.c
  src/client/packets/468.c
  src/client/packets/474.c
  src/client/packets/508.c
  src/client/packets/530.c
  src/client/packets/614.c
  src/client/packets/667.c
  src/client/packets/742.c
  src/client/packets/745.c
  src/client/packets/790.c
  src/client/packets/876.c
  src/client/packets/887.c
  src/client/packets/902.c
  src/client/packets/910.c
  src/client/poll.c
  src/client/send.c
  src/server/conn.c
  src/server/game.c
  src/server/handshake.c
  src/server/listen.c
  src/server/login.c
  src/server/packets.c
  src/server/emit.c
  src/server/packets/115.c
  src/server/packets/159.c
  src/server/packets/177.c
  src/server/packets/254.c
  src/server/packets/282.c
  src/server/packets/317.c
  src/server/packets/350.c
  src/server/packets/377.c
  src/server/packets/443.c
  src/server/packets/468.c
  src/server/packets/474.c
  src/server/packets/481.c
  src/server/packets/508.c
  src/server/packets/525.c
  src/server/packets/530.c
  src/server/packets/614.c
  src/server/packets/667.c
  src/server/packets/742.c
  src/server/packets/745.c
  src/server/packets/790.c
  src/server/packets/876.c
  src/server/packets/887.c
  src/server/packets/902.c
  src/server/packets/910.c
  src/server/poll.c
  src/server/server.c
  src/server/update.c
  src/server/util.c
)

file(GLOB RSLIB_MOVE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/both/move/*.c")
list(APPEND RSLIB_SRC ${RSLIB_MOVE})

add_library(rslib ${RSLIB_SRC})

target_include_directories(rslib
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(rslib PUBLIC ZLIB::ZLIB BZip2::BZip2 m)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(rslib PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(rslib-inspect examples/rslib_inspect.c)
target_link_libraries(rslib-inspect PRIVATE rslib)

add_executable(rslib-world examples/rslib_world.c)
target_link_libraries(rslib-world PRIVATE rslib)

if(NOT RSLIB_AS_SUBPROJECT)
enable_testing()
add_executable(test_rslib tests/test_rslib.c)
target_link_libraries(test_rslib PRIVATE rslib)
get_filename_component(RSARCHIVE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
target_compile_definitions(test_rslib PRIVATE RSARCHIVE_ROOT="${RSARCHIVE_ROOT}")
add_test(NAME test_rslib COMMAND test_rslib)

add_executable(verify_archive_refactor tests/verify_archive_refactor.c)
target_link_libraries(verify_archive_refactor PRIVATE rslib)
target_compile_definitions(verify_archive_refactor PRIVATE RSARCHIVE_ROOT="${RSARCHIVE_ROOT}")
add_test(NAME verify_archive_refactor COMMAND verify_archive_refactor)

add_executable(test_pack tests/test_pack.c)
target_link_libraries(test_pack PRIVATE rslib)
add_test(NAME test_pack COMMAND test_pack)

foreach(gap_test test_action test_asset test_cam test_content test_packets_gap test_rsc_game test_session_gap test_sync test_client_action test_era_body test_dat test_build_caps test_milestone_wire)
  add_executable(${gap_test} tests/${gap_test}.c)
  target_link_libraries(${gap_test} PRIVATE rslib)
  target_compile_definitions(${gap_test} PRIVATE RSARCHIVE_ROOT="${RSARCHIVE_ROOT}")
  add_test(NAME ${gap_test} COMMAND ${gap_test})
endforeach()
endif()

install(TARGETS rslib
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
)
install(DIRECTORY include/ DESTINATION include)

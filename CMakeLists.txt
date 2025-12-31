cmake_minimum_required(VERSION 3.15)
project(httcpp LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

# ---- curl ----
set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
set(BUILD_STATIC_CURL OFF CACHE BOOL "" FORCE)
set(CURL_BUILD_TESTING OFF CACHE BOOL "" FORCE)

if(MSVC OR WIN32)
    set(CURL_USE_SCHANNEL ON CACHE BOOL "" FORCE)
    set(CURL_USE_OPENSSL OFF CACHE BOOL "" FORCE)
elseif(UNIX)
    set(CURL_USE_SCHANNEL OFF CACHE BOOL "" FORCE)
    set(CURL_USE_OPENSSL ON CACHE BOOL "" FORCE)
endif()

set(CURL_DISABLE_DICT     ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_FTP      ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_GOPHER   ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_IMAP     ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_LDAP     ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_LDAPS    ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_MQTT     ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_POP3     ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_RTSP     ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_SMB      ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_SMTP     ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_TELNET   ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_TFTP     ON CACHE BOOL "" FORCE)

set(CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)
set(CURL_DISABLE_HSTS OFF CACHE BOOL "" FORCE)
set(CURL_DISABLE_ALTSVC ON CACHE BOOL "" FORCE)

set(CURL_INSTALL OFF CACHE BOOL "" FORCE)
set(CURL_ENABLE_EXPORT_TARGET OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/curl)
# --------------
add_library(httcpp STATIC)

target_sources(httcpp
    PRIVATE
        src/client.cpp
        src/response.cpp
)

target_link_libraries(httcpp PRIVATE CURL::libcurl)

if(MINGW)
    target_link_libraries(httcpp PRIVATE stdc++exp)
endif()

target_include_directories(httcpp
    PUBLIC
    include
)

option(httcpp_BUILD_TESTS "Build httcpp tests" ON)
if(httcpp_BUILD_TESTS)
    add_executable(httcpp_test
        tests/test.cpp
    )

    option(httcpp_BUILD_LIB "" ON)
    if (httcpp_BUILD_LIB)
        target_include_directories(httcpp_test PRIVATE include)
        target_link_libraries(httcpp_test PRIVATE httcpp)
        else()
        target_include_directories(httcpp_test PRIVATE single_include)
    endif()
endif()

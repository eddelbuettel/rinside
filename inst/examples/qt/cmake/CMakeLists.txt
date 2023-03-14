cmake_minimum_required(VERSION 3.21)

project(qtdensitydemo)
find_package(Qt6 REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets SvgWidgets)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_MESSAGE_CONTEXT_SHOW ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_FIND_FRAMEWORK ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_VERBOSE_MAKEFILE 0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_USE_QTSVG 1)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_DLL)
include_directories(${QT_INCLUDE_DIR})
link_directories(${QT_LIBRARY_DIR})

execute_process(COMMAND R RHOME
                OUTPUT_VARIABLE R_HOME)

set(sources ../main.cpp
            ../qtdensity.cpp)
             
set(headers ../qtdensity.h)

set(NUM_TRUNC_CHARS 2)

execute_process(COMMAND R CMD config --cppflags OUTPUT_VARIABLE RCPPFLAGS)
string(SUBSTRING ${RCPPFLAGS} ${NUM_TRUNC_CHARS} -1 RCPPFLAGS)
include_directories(${RCPPFLAGS})

execute_process(COMMAND R CMD config --ldflags OUTPUT_VARIABLE RLDFLAGS)
string(LENGTH ${RLDFLAGS} RLDFLAGS_LEN)
string(STRIP ${RLDFLAGS} RLDFLAGS)

execute_process(COMMAND Rscript -e "Rcpp:::CxxFlags()" OUTPUT_VARIABLE RCPPINCL)
string(SUBSTRING ${RCPPINCL} ${NUM_TRUNC_CHARS} -1 RCPPINCL)
string(REPLACE "\"" "" RCPPINCL ${RCPPINCL}) #Rcpp::CxxFlags() adds quotes around the include directory, which is friendly and all but breaks include_directories
include_directories(${RCPPINCL})

execute_process(COMMAND Rscript -e "Rcpp:::LdFlags()" OUTPUT_VARIABLE RCPPLIBS)
if (${RCPPLIBS} MATCHES "[-][L]([^ ;])+")
    string(SUBSTRING ${CMAKE_MATCH_0} ${NUM_TRUNC_CHARS} -1 RCPPLIBS_L)
    link_directories(${RCPPLIBS_L} )
endif()

if (${RCPPLIBS} MATCHES "[-][l][R]([^;])+")
    string(SUBSTRING ${CMAKE_MATCH_0} ${NUM_TRUNC_CHARS} -1 RCPPLIBS_l)
endif()

execute_process(COMMAND Rscript -e "RInside:::CxxFlags()"
                OUTPUT_VARIABLE RINSIDEINCL)
string(SUBSTRING ${RINSIDEINCL} ${NUM_TRUNC_CHARS} -1 RINSIDEINCL)
include_directories(${RINSIDEINCL})

execute_process(COMMAND Rscript -e "RInside:::LdFlags()" OUTPUT_VARIABLE RINSIDELIBS)
string(STRIP ${RINSIDELIBS} RINSIDELIBS )
string(REPLACE "\"" "" RINSIDELIBS ${RINSIDELIBS})

if (${RINSIDELIBS} MATCHES "[-][L]([^ ;])+")
    string(SUBSTRING ${CMAKE_MATCH_0} ${NUM_TRUNC_CHARS} -1 RINSIDELIBS_L)
    link_directories(${RINSIDELIBS_L})
endif()

if (${RINSIDELIBS} MATCHES "[-][l][R]([^;])+")
	string(SUBSTRING ${CMAKE_MATCH_0} ${NUM_TRUNC_CHARS} -1 RINSIDELIBS_l)
endif()

if (${RINSIDELIBS} MATCHES ".+\.a$")
	set(RINSIDELIBS_l ${RINSIDELIBS})
endif()

execute_process(COMMAND R CMD config CXXFLAGS    OUTPUT_VARIABLE RCXXFLAGS)
execute_process(COMMAND R CMD config BLAS_LIBS   OUTPUT_VARIABLE RBLAS)
execute_process(COMMAND R CMD config LAPACK_LIBS OUTPUT_VARIABLE RLAPACK)

set(CMAKE_CXX_FLAGS "-W -Wall -pedantic -Wextra ${CMAKE_CXX_FLAGS}")

if (CMAKE_BUILD_TYPE STREQUAL "DEBUG" OR
    CMAKE_BUILD_TYPE STREQUAL "RelWithDebugInfo" )
    add_definitions("-DDEBUG")
elseif ( CMAKE_BUILD_TYPE STREQUAL "RELEASE" )
    add_definitions("-O3")
endif()

qt_add_executable(qtdensitydemo ${sources} ${headers})

target_link_libraries(
	qtdensitydemo
	PRIVATE
	Qt6::Gui
	Qt6::Core
	Qt6::Widgets
	Qt6::SvgWidgets
	${BLAS_LIBS}
	${LAPACK_LIBS}
	${RCPPLIBS_l}
	${RINSIDELIBS_l}
	${RLDFLAGS}
)

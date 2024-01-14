cmake_minimum_required(VERSION 3.13.4)

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED TRUE )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17" )

#set(GLFW_BUILD_WAYLAND TRUE)
#set(GLFW_BUILD_X11 FALSE)
set(GLFW_BUILD_X11 TRUE)
project( ProjectiveTexture )

find_library( GLES2-lib GLESv2 )

include_directories( . )
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
file( GLOB SOURCES "./*.cpp" )

add_executable( ${PROJECT_NAME} ${SOURCES} )


find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

#           CHECK ALL THE SUBMODULES
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/glfw/CMakeLists.txt")
    message(FATAL_ERROR "The glfw submodules was not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()


add_subdirectory( external/glfw )
add_subdirectory( external/glm )


target_include_directories( ${PROJECT_NAME} 
	PUBLIC external/glfw/include
	PUBLIC external/glm
	PUBLIC external
	PRIVATE /usr/local/include/assimp
)

target_link_directories( ${PROJECT_NAME}
	PRIVATE external/glfw/src
	PRIVATE external/glm

	#PRIVATE /usr/local/include/assimp
)



target_link_libraries( ${PROJECT_NAME} PRIVATE 
	glfw
	glm::glm

	${GLES2-lib}
	assimp
)

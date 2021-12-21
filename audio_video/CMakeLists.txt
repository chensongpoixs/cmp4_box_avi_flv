CMAKE_MINIMUM_REQUIRED(VERSION 3.14.2)

PROJECT(audio_video)
add_compile_options(-std=c++11)

IF(WIN32)
	set(OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/win32)
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/x64)#64bit
		endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/libs)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/libs)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/bin)
	SET(OUTPUT_LIBDIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
	SET(OUTPUT_BINDIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
ELSE()


	SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin)
	MAKE_DIRECTORY(${OUTPUT_BINDIR})
	IF(MSVC AND NOT MSVC_IDE)
		MAKE_DIRECTORY(${OUTPUT_BINDIR}/${OSG_PLUGINS})
	ENDIF(MSVC AND NOT MSVC_IDE)

	#SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib/${CMAKE_SYSTEM_NAME})
	SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib)
	MAKE_DIRECTORY(${OUTPUT_LIBDIR})
	IF(NOT MSVC OR MSVC_IDE)
		MAKE_DIRECTORY(${OUTPUT_LIBDIR}/${OSG_PLUGINS})
	ENDIF(NOT MSVC OR MSVC_IDE)

	# CMake >= 2.8.1 changed the output directory algorithm (See doc).
	# Here we also set per-configuration directories (CMAKE_*_OUTPUT_DIRECTORY_<CONFIG>), or else binaries are generated in /bin/Debug and /bin/Release, etc. with MSVC and Xcode.
	# (Doc reads "multi-configuration generators (VS, Xcode) do NOT append a per-configuration subdirectory to the specified directory").
	# The workaround for 2.6.x (adding "../" as an output prefix for each target) seem to have no effect in >=2.8.1, so there is no need to change this.
	SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR})
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BINDIR})
	IF(WIN32)
		SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_BINDIR})
	ELSE(WIN32)
		SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR})
	ENDIF(WIN32)
ENDIF()

set(CMAKE_DEBUG_POSTFIX d)


set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules/)

#SET（LIBYUV_INCLUDE  "D:/Work/cmedia_server/webrtc_google/src/third_party/libyuv/include"）
#SET（LIBYUV_LIB "D:\Work\cmedia_server\webrtc_google\src\out\test_vs2017_debug\win_clang_x64\obj\third_party\libyuv"）

FIND_PACKAGE(FFmpeg)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/video
					${CMAKE_CURRENT_BINARY_DIR}	
					${FFMPEG_LIBAVDEVICE_INCLUDE_DIRS}
					D:/Work/cmedia_server/webrtc_google/src/third_party/libyuv/include
					
)	


add_subdirectory(video)
add_subdirectory(H264)






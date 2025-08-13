cmake_minimum_required(VERSION 2.8)
project(dsp_video)

set(CMAKE_CXX_FLAGS "-std=c++11  -g -Wall ")
#set(CMAKE_BUILD_TYPE "Debug")
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/linux)

############### dependencies ######################
# eigen
SET(CMAKE_VERBOSE_MAKEFILE ON)


SET(EXTRA_INC_DIR /userdata/chensong/dep) 


INCLUDE_DIRECTORIES( 
					/usr/local/include
                            ${EXTRA_INC_DIR}/mpp/include
                             ${EXTRA_INC_DIR}/ffmpeg/include 
					)
					
					
LINK_DIRECTORIES(
#				/usr/local/lib
        ${EXTRA_INC_DIR}/mpp/lib
		${EXTRA_INC_DIR}/ffmpeg/lib 
				)
				
				
file(GLOB ALL_CXX_SRCS
                    ./*.cpp 
				    ./*.h 
				)
set(ALL_LIB  	pthread
				libdl.so
				pthread
        librockchip_mpp.so
        libz.so
		libavcodec.so
		libavdevice.so
		libavfilter.so  
		libavformat.so
		libavutil.so  
		libswresample.so
		libswscale.so
       # librockchip_vpu.so
        pthread
		lzma
		drm
		#vdpau
	)	
	
add_executable(${PROJECT_NAME} ${ALL_CXX_SRCS})



target_link_libraries(${PROJECT_NAME} ${ALL_LIB})

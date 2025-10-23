

project(libmedia_codec)
cmake_minimum_required(VERSION 2.8)
MESSAGE(STATUS  "####################################")
MESSAGE(STATUS  "#############${PROJECT_NAME}###############")
MESSAGE(STATUS  "####################################")

#add_definitions(
				#	-DWIN32
				#	-D_DEBUG
				##	-D_WINDOWS
				#	-D_UNICODE
				#	-DUNICODE
				#	-D_AFXDLL
#)

 


include_directories(
	../
	../libwebrtc
	../deps/libyuv/include
	../deps/abseil-cpp/
	../deps/curl-7.81.0/include
	../deps/jsoncpp/source/include
	${DEPS_DIR}/ffmpeg/include
)



#usrsctplib

file(GLOB libmedia_codec_source 
	 
	*.h
	*.cpp 
	 *.cc 
 
 	video_codecs/*.h
	video_codecs/*.cpp 
	video_codecs/*.cc 
	
	
	
    video_codecs/svc/*.h
	video_codecs/svc/*.cpp 
	video_codecs/svc/*.cc 
	video_codecs/utility/*.h 
	video_codecs/utility/*.cc 
	video_codecs/utility/*.cpp 
	generic_frame_descriptor/*.h 
	generic_frame_descriptor/*.cpp 
	generic_frame_descriptor/*.cc 
	
	audio_codec/*.h 
	audio_codec/*.cpp 
) 


 
add_library(${PROJECT_NAME}   STATIC  ${libmedia_codec_source})
 

 

add_subdirectory(test)

#set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /SAFESEH:NO")


set(AllFile ${libmedia_codec_source} )
foreach(fileItem ${AllFile})       
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${fileItem}" DIRECTORY)
	
    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")
	#message(status   "---> fileItem = ${fileItem}     GROUP ---->  ${GROUP}")
    # Group into "Source Files" and "Header Files"
    set(GROUP "${GROUP}")
    source_group("${GROUP}" FILES "${fileItem}")
endforeach()



set_property(TARGET ${PROJECT_NAME}  				PROPERTY FOLDER libmedia) 

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
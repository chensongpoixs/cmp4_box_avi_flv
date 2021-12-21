PROJECT(cvideo)
CMAKE_MINIMUM_REQUIRED(VERSION 3.14.2)
MESSAGE(STATUS "#########################")
MESSAGE(STATUS "${PROJECT_NAME}")

file(GLOB ALL_CXX_SRCS
./*.cpp
)

add_executable(${PROJECT_NAME} ${ALL_CXX_SRCS} )


set(ALL_LIB 
		${FFMPEG_LIBAVCODEC_LIBRARIES}
        ${FFMPEG_LIBAVDEVICE_LIBRARIES}
        ${FFMPEG_LIBAVFILTER_LIBRARIES}
        ${FFMPEG_LIBAVFORMAT_LIBRARIES}
        ${FFMPEG_LIBAVUTIL_LIBRARIES}
        ${FFMPEG_LIBPOSTPROC_LIBRARIES}
        ${FFMPEG_LIBSWRESAMPLE_LIBRARIES}
        ${FFMPEG_LIBSWSCALE_LIBRARIES}
		D:/Work/cmedia_server/webrtc_google/src/out/test_vs2017_debug/win_clang_x64/obj/third_party/libyuv/libyuv_internal.lib
		#D:/Work/cmedia_server/webrtc_google/src/out/test_vs2017_debug/obj/third_party/libjpeg_turbo/libjpeg.lib
		#D:/Work/cmedia_server/webrtc_google/src/out/test_vs2017_debug/obj/third_party/libjpeg_turbo/simd.lib
		#D:/Work/cmedia_server/webrtc_google/src/out/test_vs2017_debug/obj/third_party/libjpeg_turbo/simd_asm.lib
	)
#ADD_DEPENDENCIES(${PROJECT_NAME}  ${ALL_LIB})
target_link_libraries(${PROJECT_NAME} ${ALL_LIB})
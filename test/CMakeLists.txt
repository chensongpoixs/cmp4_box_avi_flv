

project(libmedia_codec_test)
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
	../../
	../../libwebrtc
	../../deps/libyuv/include
	../../deps/abseil-cpp/
	../../deps/curl-7.81.0/include
	../../deps/jsoncpp/source/include
	${DEPS_DIR}/ffmpeg/include
)



#usrsctplib

file(GLOB   audio_aac_source 
	 
	*.h
	*.cpp 
	 *.cc 
  
	
	 
) 


 
#add_library(aac_decder  STATIC  ${audio_aac_source})
 
add_executable(aac_decder     ${audio_aac_source}      )
add_dependencies(aac_decder libmedia_codec)
#add_dependencies(${PROJECT_NAME}   libmedia_codec)
# add_dependencies(${PROJECT_NAME} libsip)

set(ALL_LIB 
 
    faad
	
	libmedia_codec
	rtc_base
	logging
	 logging

				 async_resolver_interface
				 async_socket
				 bitstream_reader
				 callback_list
				 checks
				 ip_address
				 net_helpers
				 network_constants
				 null_socket_server
				 platform_thread
				platform_thread_types
				 rate_limiter
				 rtc_base
				 rtc_base_approved
				 rtc_base_win
				 #rtc_base_win_wgc
				 rtc_event
				 rtc_numerics
				 rtc_task_queue
				 rtc_task_queue_win
				 socket
				 socket_address
				 stringutils
				 threading
				 timeutils
				 weak_ptr
				 win32

				 base64
				 sigslot 
				  iphlpapi.lib
				  libmedia_codec
				task_queue
				#libwebrtc
				async_resolver_interface
				async_socket
				#async_dns_resolver
				wrapping_async_dns_resolver
				stringutils
				common_video
		
		proxy
		rtc_pc_base
		checks
		rtc_base
		common_audio
		task_queue
		default_task_queue_factory
		rtc_base_win
		rtc_base_win_wgc
		rtc_base_approved

		rtc_media_base

		# rtc_base/third_party
		base64
		sigslot
		# rtc_base/system
		file_wrapper
		default_task_queue_factory

		rtc_task_queue_win
		# api/video

		video_rtp_headers 						
		video_frame 							
		video_frame_i010    					
		#recordable_encoded_frame 				
		#video_frame_type  						
		#render_resolution 						
		encoded_image 							
		encoded_frame 							
		rtp_video_frame_assembler 				
		#video_codec_constants  				
		video_bitrate_allocation 				
		#video_layers_allocation 				
		video_bitrate_allocator 				
		#video_bitrate_allocator_factory 		
		#video_stream_decoder 					
		video_stream_decoder_create 			
		video_adaptation 						
		#video_stream_encoder 					
		video_frame_metadata 					
		builtin_video_bitrate_allocator_factory


		# api 
		media_stream_interface

			Imm32 
			comctl32
			ws2_32.lib
			winmm.lib


			libyuv
		libcrypto.lib 
		libssl.lib 
		#
		##libredis.lib
		#libzmalloc.lib
		iphlpapi.lib
		#crypto32.lib 
		ws2_32   
		crypt32
		Shell32.lib
		 advapi32
              iphlpapi
              psapi
              shell32
              user32
              userenv
              ws2_32
			  mswsock.lib
			  #  webrtc  timeGetTime  
			  winmm.lib
			  #ucrt.lib
			  #  libuv.lib  µÄÒÀÀµ¿â
			  dbghelp.lib

			  # rtc_base/synchronization
		yield                       
		mutex               		
		sequence_checker_internal 	
		yield_policy

		#rtc_base

		logging
		ip_address
		socket_address
		#rtc_base
		network_constants
		async_resolver_interface
		async_socket            
		null_socket_server      
		threading               
		socket 
		stringutils
		timeutils
		platform_thread
		platform_thread_types
		net_helpers
		rtc_event
		win32 
		rtc_task_queue
		# api/ numerics
		numerics
		# api/video_codecs 
		video_codecs_api  				
		#bitstream_parser_api  			
		builtin_video_decoder_factory  	
		builtin_video_encoder_factory  	
		vp8_temporal_layers_factory  	
		rtc_software_fallback_wrappers  
		#api
		wrapping_async_dns_resolver
		libjingle_peerconnection_api
		#async_dns_resolver
		#packet_socket_factory
		#scoped_refptr

		#api
		rtc_error
		#api /rtc_event_log 
		rtc_event_log
		rtc_event_log_factory

		# logging

		#rtc_event_log_api  		   		
		rtc_event_field  		   		
		rtc_stream_config  		   		
		rtc_event_pacing  		   		
		rtc_event_audio  		   		
		rtc_event_bwe  		   			
		rtc_event_frame_events  		
		rtc_event_generic_packet_events 
		rtc_event_rtp_rtcp  		   	
		rtc_event_video      		   	
		rtc_event_number_encodings  	
		rtc_event_log_impl_encoder  	
		#rtc_event_log_impl_output  		
		fake_rtc_event_log  		   	
		ice_log  	
		
		#rtc_base/experiments 
		#alr_experiment                             
		field_trial_parser                         
		quality_rampup_experiment                  
		quality_scaler_settings                    
		bandwidth_quality_scaler_settings          
		quality_scaling_experiment                 
		normalize_simulcast_size_experiment        
		balanced_degradation_settings              
		cpu_speed_experiment                       
		encoder_info_settings                      
		rtt_mult_experiment                        
		jitter_upper_bound_experiment              
		rate_control_settings                      
		keyframe_interval_settings_experiment      
		stable_target_rate_experiment              
		min_video_bitrate_experiment 
		# rtc_base

		#call 

		version                  
		call_interfaces          
		#audio_sender_interface   
		rtp_interfaces           
		call_rtp_receiver             
		call_rtp_sender               
		bitrate_configurator     
		bitrate_allocator        
		call                     
		#receive_stream_interface 
		video_stream_api         
		simulated_network        
		fake_network   
		
		#pc 
		session_description  				
		simulcast_description  				
		media_protocol_names  				
		connection_context  				
		peer_connection_message_handler  	
		usage_pattern  						
		rtp_transceiver  					
		rtp_transmission_manager  			
		transceiver_list  					
		pc_rtp_receiver  						
		audio_rtp_receiver  				
		video_rtp_receiver  				
		video_rtp_track_source  			
		audio_track  						
		video_track  						
		#sdp_state_provider  				
		jitter_buffer_delay  				
		remote_audio_source  				
		pc_rtp_sender  						
		rtp_parameters_conversion  			
		dtmf_sender  						
		media_stream  						
		video_track_source  	
			  # rtc_base/third_party
		base64.lib
		sigslot.lib 
		# rtc_base/system
		file_wrapper.lib
		# rtc_base/synchronization
		yield                       
		mutex               		
		sequence_checker_internal 	
		yield_policy
		#rtc_base/memory 
		aligned_malloc  
		fifo_buffer  	
		#rtc_base
		callback_list
		rtc_numerics
		logging
		ip_address
		socket_address
		 crypt32.lib
		iphlpapi.lib
       secur32.lib
		#rtc_base
		rtc_base_tests_utils
		weak_ptr
		network_constants
		async_resolver_interface
		async_socket            
		null_socket_server      
		threading               
		socket 
		stringutils
		timeutils
		platform_thread
		platform_thread_types
		net_helpers
		rtc_event
		win32 
		rtc_base
		rate_limiter
		#api
		wrapping_async_dns_resolver
		#stats_collector_interface  
		bitstream_reader

		libice
		libp2p_peerconnection 

		libmedia_transfer_protocol
		libsrtp2
		libmedia_codec
		#libx264.lib
		x264

		
		#libx264.lib
		#libx264d.lib
	#libx264.dll.lib
	#libx264.lib

	# moudles/audio_device 
	audio_device_buffer 						
	audio_device_generic						
	audio_device_name_source 					
	windows_core_audio_utility 					
	audio_device_module_from_input_and_output 	
	audio_device_impl 			
	desktop_capture_differ_sse2
	desktop_capture_generic
	primitives


	d3d11.lib
      dxgi.lib
	jpeg-static
		

		#curl 
		jsoncpp_static
	       dmoguids.lib
          wmcodecdspuuid.lib
        amstrmid.lib
         msdmo.lib
        oleaut32.lib
		opus
		
libcross_platform_collection_render
		video_capture_module
		video_capture_internal_impl
		ole32.lib
        oleaut32.lib
        strmiids.lib
        user32.lib
		libwebrtc
		abseil_dll
		absl_cordz_functions
		absl_cordz_handle
		absl_cordz_info
		absl_cordz_sample_token
		absl_flags
		absl_flags_commandlineflag_internal
		absl_flags_internal
		absl_flags_marshalling
		absl_flags_parse
		absl_flags_private_handle_accessor
		absl_flags_program_name
		absl_flags_reflection
		absl_flags_usage
		absl_flags_usage_internal
		absl_low_level_hash
		absl_random_internal_distribution_test_util
		jpeg-static
		

		#curl 
		jsoncpp_static
		#libcurl
		# curl 
		Ws2_32.lib
		Wldap32.lib
		# api
		audio_options_api
		
		api
		peerconnection

		#  moudles /video /desktop_capture

		desktop_capture_generic
		primitives
		desktop_capture_differ_sse2

		d3d11.lib
      dxgi.lib
		

		#common_video 

		common_video
		
		proxy
		rtc_pc_base
		checks
		rtc_base
		common_audio
		task_queue
		default_task_queue_factory
		rtc_base_win
		rtc_base_win_wgc
		rtc_base_approved

		rtc_media_base

		# rtc_base/third_party
		base64
		sigslot
		# rtc_base/system
		file_wrapper
		default_task_queue_factory

		rtc_task_queue_win
		# api/video

		video_rtp_headers 						
		video_frame 							
		video_frame_i010    					
		#recordable_encoded_frame 				
		#video_frame_type  						
		#render_resolution 						
		encoded_image 							
		encoded_frame 							
		rtp_video_frame_assembler 				
		#video_codec_constants  				
		video_bitrate_allocation 				
		#video_layers_allocation 				
		video_bitrate_allocator 				
		#video_bitrate_allocator_factory 		
		#video_stream_decoder 					
		video_stream_decoder_create 			
		video_adaptation 						
		#video_stream_encoder 					
		video_frame_metadata 					
		builtin_video_bitrate_allocator_factory


		# api 
		media_stream_interface

			Imm32 
			comctl32
			ws2_32.lib
			winmm.lib


			libyuv
		libcrypto.lib 
		libssl.lib 
		#
		##libredis.lib
		#libzmalloc.lib
		iphlpapi.lib
		#crypto32.lib 
		ws2_32   
		crypt32
		Shell32.lib
		 advapi32
              iphlpapi
              psapi
              shell32
              user32
              userenv
              ws2_32
			  mswsock.lib
			  #  webrtc  timeGetTime  
			  winmm.lib
			  #ucrt.lib
			  #  libuv.lib  µÄÒÀÀµ¿â
			  dbghelp.lib

			  # rtc_base/synchronization
		yield                       
		mutex               		
		sequence_checker_internal 	
		yield_policy

		#rtc_base

		logging
		ip_address
		socket_address
		#rtc_base
		network_constants
		async_resolver_interface
		async_socket            
		null_socket_server      
		threading               
		socket 
		stringutils
		timeutils
		platform_thread
		platform_thread_types
		net_helpers
		rtc_event
		win32 
		rtc_task_queue
		# api/ numerics
		numerics
		# api/video_codecs 
		video_codecs_api  				
		#bitstream_parser_api  			
		builtin_video_decoder_factory  	
		builtin_video_encoder_factory  	
		vp8_temporal_layers_factory  	
		rtc_software_fallback_wrappers  
		#api
		wrapping_async_dns_resolver
		libjingle_peerconnection_api
		#async_dns_resolver
		#packet_socket_factory
		#scoped_refptr

		#api
		rtc_error
		#api /rtc_event_log 
		rtc_event_log
		rtc_event_log_factory

		# logging

		#rtc_event_log_api  		   		
		rtc_event_field  		   		
		rtc_stream_config  		   		
		rtc_event_pacing  		   		
		rtc_event_audio  		   		
		rtc_event_bwe  		   			
		rtc_event_frame_events  		
		rtc_event_generic_packet_events 
		rtc_event_rtp_rtcp  		   	
		rtc_event_video      		   	
		rtc_event_number_encodings  	
		rtc_event_log_impl_encoder  	
		#rtc_event_log_impl_output  		
		fake_rtc_event_log  		   	
		ice_log  	
		
		#rtc_base/experiments 
		#alr_experiment                             
		field_trial_parser                         
		quality_rampup_experiment                  
		quality_scaler_settings                    
		bandwidth_quality_scaler_settings          
		quality_scaling_experiment                 
		normalize_simulcast_size_experiment        
		balanced_degradation_settings              
		cpu_speed_experiment                       
		encoder_info_settings                      
		rtt_mult_experiment                        
		jitter_upper_bound_experiment              
		rate_control_settings                      
		keyframe_interval_settings_experiment      
		stable_target_rate_experiment              
		min_video_bitrate_experiment 
		# rtc_base

		#call 

		version                  
		call_interfaces          
		#audio_sender_interface   
		rtp_interfaces           
		call_rtp_receiver             
		call_rtp_sender               
		bitrate_configurator     
		bitrate_allocator        
		call                     
		#receive_stream_interface 
		video_stream_api         
		simulated_network        
		fake_network   
		
		#pc 
		session_description  				
		simulcast_description  				
		media_protocol_names  				
		connection_context  				
		peer_connection_message_handler  	
		usage_pattern  						
		rtp_transceiver  					
		rtp_transmission_manager  			
		transceiver_list  					
		pc_rtp_receiver  						
		audio_rtp_receiver  				
		video_rtp_receiver  				
		video_rtp_track_source  			
		audio_track  						
		video_track  						
		#sdp_state_provider  				
		jitter_buffer_delay  				
		remote_audio_source  				
		pc_rtp_sender  						
		rtp_parameters_conversion  			
		dtmf_sender  						
		media_stream  						
		video_track_source  	
			  # rtc_base/third_party
		base64.lib
		sigslot.lib 
		# rtc_base/system
		file_wrapper.lib
		# rtc_base/synchronization
		yield                       
		mutex               		
		sequence_checker_internal 	
		yield_policy
		#rtc_base/memory 
		aligned_malloc  
		fifo_buffer  	
		#rtc_base
		callback_list
		rtc_numerics
		logging
		ip_address
		socket_address
		 crypt32.lib
		iphlpapi.lib
       secur32.lib
		#rtc_base
		rtc_base_tests_utils
		weak_ptr
		network_constants
		async_resolver_interface
		async_socket            
		null_socket_server      
		threading               
		socket 
		stringutils
		timeutils
		platform_thread
		platform_thread_types
		net_helpers
		rtc_event
	)

target_link_libraries(aac_decder   ${ALL_LIB} )

#set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /SAFESEH:NO")


set(AllFile ${audio_aac_source} )
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



set_property(TARGET aac_decder 				PROPERTY FOLDER libmedia_codec/test) 

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
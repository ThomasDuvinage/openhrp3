
# @author Shin'ichiro Nakaoka

include(${OPENHRP_IDL_DIR}/OpenHRP/IDL_FILE_BASENAMES.cmake)

set(idl_flags_cpp ${VERBOSE_FLAG} -bcxx -Wbuse_quotes -Wbh=.hh -Wbs=Sk.cpp -I${OPENRTM_IDL_DIR})
set(idl_flags_py  ${VERBOSE_FLAG} -bpython -I${OPENRTM_IDL_DIR})

if( NOT ADDITIONAL_SYMBOL STREQUAL "")
  set(idl_flags_cpp ${idl_flags_cpp} -D ${ADDITIONAL_SYMBOL})
  set(idl_flags_py  ${idl_flags_py}  -D ${ADDITIONAL_SYMBOL})
endif()


foreach(name ${IDL_FILE_BASENAMES})
  set(idl_files ${idl_files} ${OPENHRP_IDL_DIR}/OpenHRP/${name}.idl)
  set(idl_cpp_files ${idl_cpp_files} ${CMAKE_CURRENT_BINARY_DIR}/${name}Sk.cpp)
  set(idl_h_files ${idl_h_files} ${CMAKE_CURRENT_BINARY_DIR}/${name}.hh)
  set(idl_py_files ${idl_py_files} ${CMAKE_CURRENT_BINARY_DIR}/${name}_idl.py)
endforeach()

set(idl_py_depends)
foreach(idl_basename ${IDL_FILE_BASENAMES})
  set(idl_file ${OPENHRP_IDL_DIR}/OpenHRP/${idl_basename}.idl)
  if(UNIX)
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${idl_basename}.hh ${CMAKE_CURRENT_BINARY_DIR}/${idl_basename}Sk.cpp
      COMMAND omniidl ${idl_flags_cpp} ${idl_file}
      DEPENDS ${idl_files}
      )
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${idl_basename}_idl.py
      COMMAND omniidl ${idl_flags_py} ${idl_file}
      DEPENDS ${idl_files} ${idl_py_depends}
      )
  elseif(WIN32)
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${idl_basename}.hh ${CMAKE_CURRENT_BINARY_DIR}/${idl_basename}Sk.cpp
      COMMAND set PATH= "${OMNIORB_DIR}/bin/x86_win32"\;%PATH%
      COMMAND omniidl ${idl_flags_cpp} ${idl_file}
      DEPENDS ${idl_files}
      )
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${idl_basename}_idl.py
      COMMAND set PATH= "${OMNIORB_DIR}/bin/x86_win32"\;%PATH%
      COMMAND omniidl ${idl_flags_py} ${idl_file}
      DEPENDS ${idl_files} ${idl_py_depends}
      )
  endif()
  set(idl_py_depends ${idl_py_depends} ${CMAKE_CURRENT_BINARY_DIR}/${idl_basename}_idl.py)
endforeach()

set(target hrpCorbaStubSkel-${OPENHRP_LIBRARY_VERSION})

if (COMPILE_PYTHON_STUFF)
  add_library(${target} STATIC ${idl_cpp_files} ${idl_py_files})
else()
  add_library(${target} STATIC ${idl_cpp_files})
endif()

if(UNIX)
  set_target_properties(${target} PROPERTIES COMPILE_FLAGS -fPIC)
endif()

if(WIN32)
  set_target_properties(${target} PROPERTIES DEBUG_POSTFIX d)
endif() 

if(WIN32)
install(TARGETS ${target}
	RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin CONFIGURATIONS Release Debug
	LIBRARY DESTINATION ${PROJECT_BINARY_DIR}/lib CONFIGURATIONS Release Debug
	ARCHIVE DESTINATION ${PROJECT_BINARY_DIR}/lib CONFIGURATIONS Release Debug
)
endif()

install(TARGETS ${target}
	RUNTIME DESTINATION bin CONFIGURATIONS Release Debug RelWithDebInfo
	LIBRARY DESTINATION lib CONFIGURATIONS Release Debug RelWithDebInfo
	ARCHIVE DESTINATION lib CONFIGURATIONS Release Debug RelWithDebInfo
)

set(given_headers ORBwrap.h)

install(FILES ${given_headers} ${idl_h_files} DESTINATION ${RELATIVE_HEADERS_INSTALL_PATH}/hrpCorba)
execute_process(
  COMMAND python3 -c "import sys; print('%s.%s'%(sys.version_info[0:2]))"
  OUTPUT_VARIABLE python_version
  OUTPUT_STRIP_TRAILING_WHITESPACE)

if (COMPILE_PYTHON_STUFF)
  install(FILES ${idl_py_files} DESTINATION lib/python${python_version}/dist-packages)
  set(module_py_files ${CMAKE_CURRENT_BINARY_DIR}/OpenHRP/__init__.py)
  install(FILES ${module_py_files} DESTINATION lib/python${python_version}/dist-packages/OpenHRP)
  set(module_poa_py_files ${CMAKE_CURRENT_BINARY_DIR}/OpenHRP__POA/__init__.py)
  install(FILES ${module_poa_py_files} DESTINATION lib/python${python_version}/dist-packages/OpenHRP__POA)
endif()

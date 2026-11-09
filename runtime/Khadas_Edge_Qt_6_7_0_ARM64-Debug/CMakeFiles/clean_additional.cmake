# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "app/CMakeFiles/qgst-goen220_autogen.dir/AutogenUsed.txt"
  "app/CMakeFiles/qgst-goen220_autogen.dir/ParseCache.txt"
  "app/qgst-goen220_autogen"
  "lib/gsp/Goen220GyroPlatformHost/CMakeFiles/Goen220GyroPlatformHost_autogen.dir/AutogenUsed.txt"
  "lib/gsp/Goen220GyroPlatformHost/CMakeFiles/Goen220GyroPlatformHost_autogen.dir/ParseCache.txt"
  "lib/gsp/Goen220GyroPlatformHost/Goen220GyroPlatformHost_autogen"
  "lib/gsp/Goen220Vdb/CMakeFiles/Goen220Vdb_autogen.dir/AutogenUsed.txt"
  "lib/gsp/Goen220Vdb/CMakeFiles/Goen220Vdb_autogen.dir/ParseCache.txt"
  "lib/gsp/Goen220Vdb/Goen220Vdb_autogen"
  )
endif()

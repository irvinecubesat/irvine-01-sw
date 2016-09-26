#
# Generate files containing the version number.  
# 
#
set (irvine-01_VERSION_MAJOR 00)
set (irvine-01_VERSION_MINOR 00)
set (irvine-01_VERSION_BUILD 05)
string(TIMESTAMP Build_Timestamp "%Y-%m-%d %H:%M:%S")
configure_file (
  "${PROJECT_SOURCE_DIR}/version/issue.in"
  "${PROJECT_SOURCE_DIR}/version/issue"
  )
#
# Install to /etc/ directory
#
INSTALL(FILES issue DESTINATION /etc/ COMPONENT config) 

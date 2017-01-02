#
# Install the scripts that go on the board
#

INSTALL(FILES solve-field.sh DESTINATION /usr/local/bin/ COMPONENT binaries PERMISSIONS OWNER_READ OWNER_EXECUTE)
INSTALL(FILES monitor.sh DESTINATION /usr/local/bin/ COMPONENT binaries PERMISSIONS OWNER_READ OWNER_EXECUTE)


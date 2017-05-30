#
# Install the scripts that go on the board
#

INSTALL(FILES cmdTest DESTINATION /usr/local/cmds/ COMPONENT binaries PERMISSIONS OWNER_READ OWNER_EXECUTE)
INSTALL(FILES cmdChmod DESTINATION /usr/local/cmds/ COMPONENT binaries PERMISSIONS OWNER_READ OWNER_EXECUTE)
INSTALL(FILES  cmdMtCtl cmdPowerMon cmdSnap cmdLslah cmdPwrSens cmdTest cmdUvccapture cmdCksum cmdMkdir cmdRm DESTINATION /usr/local/cmds/ COMPONENT binaries PERMISSIONS OWNER_READ OWNER_EXECUTE)
INSTALL(FILES log.sh DESTINATION /usr/local/cmds/ COMPONENT binaries)


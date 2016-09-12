# irvine-01-sw
irvine-01-sw software repository development tree

The irvine-01-sw project is the main entry point for irvine cubesat development

To get started:

1. git clone https://github.com/irvinecubesat/irvine-01-sw
2. cd irvine-01-sw
3. make all-arm

This will build the arm binaries and put them under build-arm

You can type "make all" to build x86 binaries under "build"

The first time you run the all-arm build, it will download and install the
toolchain.

CalPoly Documentation is available at https://asof.atl.calpoly.edu/trac/polysat_documentation/wiki

*Note:  Do not follow the CalPoly buildroot instructions*

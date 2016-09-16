# irvine-01-sw
irvine-01-sw software repository development tree

The irvine-01-sw project is the main entry point for irvine cubesat development

* irvine-01-sw project and toolchain install

The irvine-01-sw utilizes CMake to build the software.  You can build the
software for your host machine to test it, then cross compile it for the arm architecture in order to run it on the board.

To get started, retrieve the man software project and install the arm toolchain.
```
git clone https://github.com/irvinecubesat/irvine-01-sw
cd irvine-01-sw
make all-arm
```

This will build the arm binaries and put them under the `build-arm` directory.

You can type `make all ` to build x86 binaries under the `build` directory.

The first time you run the all-arm build, it will download and install the
toolchain.

For the next step, generate keys to be able to build the BuildRoot.
Inside the irvine-01-sw project, type:
```
make genKeys
```
This should generate keys in `~/.ssh/`  Send the `.cert` file to the system admin.
Do not send the .key file which you should keep secure.
Once he has updated the configuration with your certificate, go to the next
step to setup your buildroot environment.  Note that you can do all your
programming in the irvine-01-sw project without building the buildroot.

* BuildRoot

The BuildRoot environment builds the image to upload to the computer on the
cubesat.

clone the irvinecubsat project from github:

```
git clone https://github.com/irvinecubesat/buildroot
```

Then make it:

```
make
```
This will take a long time, and the end result will be under output/images

** Examining the image
The image will end up in the directory `buildroot/output/images/fsw_image`.
You can examine the directory structure of the image by looking at the
`fsw_image.rootfs.cpio` file:

Inside the buildroot/output/images/fsw_image directory, type:
```
mkdir tmp
cd tmp
cpio -i < ../fsw_image.rootfs.cpio
```
The cpio command will extract the rootfs into the tmp directory where you
can take a look at what is in the image.

* CalPoly Documentation
CalPoly Documentation is available at https://asof.atl.calpoly.edu/trac/polysat_documentation/wiki

*Note:  Do not follow the CalPoly buildroot instructions*

Use the above BuildRoot instructions instead.

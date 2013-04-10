# Glakkeclock

## Important
This project is no longer being actively developed but most features should be working as intended. For an alternative, see AMDOverdriveCtrl (https://github.com/CRogers/AMDOverdriveControlled).

## Websites
GitHub: https://github.com/ekroth/glakkeclock

SourceForge: https://sourceforge.net/projects/glakkeclock

AUR/makepkg: https://aur.archlinux.org/packages.php?ID=48488

## Source
	The primary place for up-to-date source code is at GitHub.
	I'll try to keep the SourceForge page somewhat up-to-date with "bigger" releases.

## Binaries (SourceForge)
### Windows binaries
	I have compiled a binary for Windows, I will try to keep this somewhat up-to-date.
	Binary should work for both x86 and x86_64.

## Linux binaries
	I won't be supplying any Linux binaries.

## Build
### Dependencies
- AMD Display Library (ADL) SDK
	http://developer.amd.com/gpu/adlsdk/Pages/default.aspx

	I'm not allowed to distribute the ADL SDK. Instead, 
	download their SDK/headers from the website above.

	Please extract the AMD ADL SDK files, and put the files in
	<ADL_SDK>/include in <GlakkeClock>/src/ADL

	AMD/ATI official Linux drivers (fglrx) 
	Can be found on http://www.amd.com, might require
	specific steps for various distributions.
	Please note that the open-source drivers will _NOT_ work!

### Build
Now cd into root directory and run following commands:

mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release

make

### Install
make install

## Bugs
You've found a bug? Impossible this software is bug free.
But if you're sure, please send me an e-mail or post bug on 
SourceForge/GitHub. Be sure to use --debug parameter.

## Contact
glakke1 at [please not spam] gmail dot com

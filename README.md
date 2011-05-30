# Glakkeclock

## Source
	The primary place for up-to-date source code is at GitHub:
	https://github.com/Glakke/glakkeclock
	I'll try to keep the SourceForge page somewhat up-to-date with "bigger" releases.

## Binaries
### Windows binaries
	I have compiled a binary for Windows, I will try to keep this somewhat up-to-date.
	The binary hasn't been tested on x86 Windows, only x86_64. (No worries
	the worst thing that could happen is that the application won't start.)

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

	### makepkg users (Arch Linux for instance)
		AUR package available at https://aur.archlinux.org/packages.php?ID=48488
		This will be the "easiest" way to build and install.

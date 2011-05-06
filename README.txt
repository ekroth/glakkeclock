> -- Instructions to build and install manually

* Dependencies

- AMD Display Library (ADL) SDK
http://developer.amd.com/gpu/adlsdk/Pages/default.aspx

I'm not allowed to distribute the ADL SDK. Instead, 
download their SDK/headers from the website above.

Please extract the AMD ADL SDK files, and put the files in
<ADL_SDK>/include in <GlakkeClock>/src/ADL

- AMD/ATI official Linux drivers (fglrx) 
Can be found on http://www.amd.com, might require
specific steps for various distributions.
Please note that the open-source drivers will _NOT_ work!

* Build

Now cd into root directory and run following commands:

mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release
make


* Install
make install


> -- makepkg users (Arch Linux for instance)

* AUR package available at https://aur.archlinux.org/packages.php?ID=48488
This will be the "easiest" way to build and install.

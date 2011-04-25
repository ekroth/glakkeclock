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
cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DKKE_ARCH=<x86 OR x86_64> -DCMAKE_BUILD_TYPE=Release
make


* Install
make install


> -- makepkg users (Arch Linux for instance)

* By using makepkg, you won't have to git clone yourself,
the script will manage this. This also means that you could
just download the PKGBUILD alone (and not the whole git).

* Copy/Move the PKGBUILD file to another directory, 
and run 'makepkg', this will build a package. This
will download AMD Display Library (ADL) as well, everything
is automated.

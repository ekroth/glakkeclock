-- Instructions to build and install manually

AMD Display Library (ADL) SDK
http://developer.amd.com/gpu/adlsdk/Pages/default.aspx

I'm not allowed to distribute the AMD ADL SDK. Instead, 
download their library from the website above.

Please extract the AMD ADL SDK files, and put the files in
<ADL_SDK>/include in <GlakkeClock>/src/ADL

Now cd into root directory and run following commands:

mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DKKE_ARCH=<x86 OR x86_64> -DCMAKE_BUILD_TYPE=Release
make

Install
make install

-- makepkg users (Arch Linux for instance)
Copy/Move the PKGBUILd file to another directory, 
and run 'makepkg', this will build a package. This
will download AMD Display Library (ADL) as well, everything
is automated.

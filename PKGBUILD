# Maintainer: Glaucous <glakke1@gmail.com>

pkgname=glakkeclock-git
pkgver=20110425
pkgrel=2
pkgdesc="AMD/ATI graphics card utility."
url="https://sourceforge.net/projects/glakkeclock/"
arch=('x86_64' 'i686')
license=('GPLv3')
depends=('catalyst-utils' 'lib32-catalyst-utils')
optdepends=()
makedepends=('cmake>=2.8' 'git')
provides=('glakkeclock')
conflicts=('glakkeclock')
replaces=('glakkeclock')
backup=()
source=('http://download2-developer.amd.com/amd/GPU/zip/ADL_SDK_3.0.zip')
md5sums=('cf380f94502b6a2860c11b8fd6b19a3c')


_gitroot='git://glakkeclock.git.sourceforge.net/gitroot/glakkeclock/glakkeclock'
_gitname='glakkeclock'
_buildtype=$CARCH

build() {
	unset LDFLAGS
	
	if [[ $CARCH = "x86_64" ]]; then
		unset depends[1]
	elif [[ $CARCH = "i686" ]]; then
		unset depends[0]
		_buildtype="x86"
	fi

	cd ${srcdir}

    msg "Connecting to the GIT server.."
    if [[ -d ${srcdir}/${_gitname} ]] ; then
        cd ${_gitname}
        git pull origin
        msg "The local files are updated."
    else
        git clone ${_gitroot}
    fi
    
    msg "GIT checkout done."
    
    if [[ -d ${srcdir}/${_gitname}-build ]]; then
       msg "Cleaning the previous build directory.." 
       rm -rf ${srcdir}/${_gitname}-build
    fi

	msg "Cloning git.."

    git clone ${srcdir}/${_gitname} ${srcdir}/${_gitname}-build

	msg "Moving ATI/AMD ADL include files to src.."
	mkdir -p $srcdir/${_gitname}-build/src/ADL
	mv ${srcdir}/include/* $srcdir/${_gitname}-build/src/ADL

	msg "Running cmake and make.."
	cd ${srcdir}/${_gitname}-build
	cmake . -DCMAKE_INSTALL_PREFIX=/usr -DKKE_ARCH=${_buildtype} -DCMAKE_BUILD_TYPE=Release
	make
	make DESTDIR=${pkgdir} install
}

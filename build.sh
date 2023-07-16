#!/bin/bash
# wget -O build.sh https://raw.githubusercontent.com/DerKleinePunk/carnine/master/build.sh
# chmod +x build.sh

source ./logger.sh
SCRIPTENTRY

echo try to download and build CarNiNe
INFO "try to download and build CarNiNe"
reproBuild="false"
if [ -d .git ]; then
	echo "called inside repro"
	reproBuild="true"
fi

echo "reproBuild $reproBuild"
INFO "reproBuild $reproBuild"

if [ "$reproBuild" = "false" ] ; then
	wget -O DebianPackages.txt https://raw.githubusercontent.com/DerKleinePunk/carnine/master/DebianPackages.txt
	exitCode=$?
	if [ $exitCode -ne 0 ] ; then
		echo "wget give an error"
		exit $exitCode
	fi
else
	echo "Don't forget git pull bevor building"
	INFO "Don't forget git pull bevor building"
fi

InstallPackage(){
	packageName="$1"
	PKG_OK=$(dpkg-query -W --showformat='${Status}\n' $packageName|grep "install ok installed")
	DEBUG "Checking for $packageName: $PKG_OK"
	if [ "" = "$PKG_OK" ]; then
		echo "No $packageName. Setting up $packageName."
		DEBUG "No $packageName. Setting up $packageName."
		sudo apt-get --yes install $packageName
	fi
}

InstallPackage git
InstallPackage git-lfs

DEPENSFILE="DebianPackages.txt"

while read LINE; do
     InstallPackage $LINE
done < $DEPENSFILE

sudo apt-get --yes update
sudo apt-get --yes upgrade
sudo apt-get autoremove -y

rpiversion=$(cat /sys/firmware/devicetree/base/compatible |cut -c1-13)
echo "PI $rpiversion"
DEBUG "PI $rpiversion"

InstallSDLComponent(){
	# https://github.com/libsdl-org/SDL/releases/download/release-2.28.1/SDL2-2.28.1.tar.gz
	packageName="$1"
	packageVersion="$2"
	packageDirName="$3-${packageVersion}"
	packageFileName="${packageDirName}.tar.gz"
	if [ ! -d "$packageDirName" ]; then
		wget -N https://github.com/libsdl-org/$packageName/releases/download/release-$packageVersion/$packageFileName
		exitCode=$?
		if [ $exitCode -ne 0 ] ; then
		   echo "wget give an Error"
		   exit $exitCode
		fi
		tar -xzf $packageFileName
		exitCode=$?
		if [ $exitCode -ne 0 ] ; then
		   echo "tar give an Error"
		   exit $exitCode
		fi
		cd $packageDirName
		./autogen.sh
		./configure
		make -j$(nproc)
		sudo make install
		cd ..
	else
		cd $packageDirName
		./autogen.sh
		./configure
		make -j$(nproc)
		sudo make install
		cd ..
	fi
}

if [ "$reproBuild" = "true" ] ; then
  cd ..
fi

echo "we are here"
pwd

DIRECTORY="SDL2-2.28.1"
if [ ! -d "$DIRECTORY" ]; then
	wget -N https://github.com/libsdl-org/SDL/releases/download/release-2.28.1/$DIRECTORY.tar.gz
	exitCode=$?
	if [ $exitCode -ne 0 ] ; then
	   echo "wget give an Error"
	   ERROR "wget give an Error"
	   exit $exitCode
	fi
	tar -xzf $DIRECTORY.tar.gz
	exitCode=$?
	if [ $exitCode -ne 0 ] ; then
	   echo "tar give an Error"
	   ERROR "wget give an Error"
	   exit $exitCode
	fi
fi

cd $DIRECTORY
./autogen.sh
if [ "$rpiversion" == "raspberrypi,4" ] ; then
	./configure --enable-video-kmsdrm --disable-esd --disable-video-wayland --disable-video-opengl --disable-video-rpi --disable-kmsdrm-shared --enable-arm-neon
elif [ "$rpiversion" == "raspberrypi,3" ] ; then
	./configure --disable-esd --disable-video-wayland --disable-video-opengl --enable-arm-neon
else
	./configure
fi
exitCode=$?
if [ $exitCode -ne 0 ] ; then
   echo "configure give an Error"
   exit $exitCode
fi
make -j$(nproc)
sudo make install
cd ..

InstallSDLComponent SDL_image 2.6.3 SDL2_image
InstallSDLComponent SDL_mixer 2.6.3 SDL2_mixer
InstallSDLComponent SDL_net 2.2.0 SDL2_net
InstallSDLComponent SDL_ttf 2.20.2 SDL2_ttf

DIRECTORY="libosmscout"
if [ ! -d "$DIRECTORY" ]; then
	git clone https://github.com/Framstag/libosmscout.git
	exitCode=$?
	if [ $exitCode -ne 0 ] ; then
	   echo "git give an Error"
	   exit $exitCode
	fi
	cd libosmscout
	git checkout 8cd316736da94541f272bd3ab8e9f0eb4750e73c
	exitCode=$?
	if [ $exitCode -ne 0 ] ; then
	   echo "git give an Error"
	   exit $exitCode
	fi
else
	cd libosmscout
	git reset --hard
	git pull
	rm OSMScout2/translations/cs.ts
	rm OSMScout2/translations/en.ts
	git checkout 8cd316736da94541f272bd3ab8e9f0eb4750e73c
	exitCode=$?
	if [ $exitCode -ne 0 ] ; then
	   echo "git give an Error"
	   exit $exitCode
	fi
fi

echo buildding libosmscout debug
DIRECTORY="build"
if [ ! -d "$DIRECTORY" ]; then
	mkdir $DIRECTORY
fi
cd $DIRECTORY
rm CMakeCache.txt
if [ "$rpiversion" == "raspberrypi,4" ] ; then
	cmake .. -DCMAKE_BUILD_TYPE=Debug -DOSMSCOUT_ENABLE_SSE=ON -DOSMSCOUT_BUILD_DOC_API=OFF -DOSMSCOUT_BUILD_DEMOS=OFF -DOSMSCOUT_BUILD_TOOL_OSMSCOUTOPENGL=OFF -DOSMSCOUT_BUILD_IMPORT=OFF -DOSMSCOUT_BUILD_CLIENT_QT=OFF -DOSMSCOUT_BUILD_TOOL_OSMSCOUT2=OFF -DOSMSCOUT_BUILD_TESTS=OFF -DCMAKE_CXX_FLAGS="-Wno-psabi" -Wno-dev
elif [ "$rpiversion" == "raspberrypi,3" ] ; then
    cmake .. -DCMAKE_BUILD_TYPE=Debug -DOSMSCOUT_ENABLE_SSE=ON -DOSMSCOUT_BUILD_DOC_API=OFF -DOSMSCOUT_BUILD_DEMOS=OFF -DOSMSCOUT_BUILD_TOOL_OSMSCOUTOPENGL=OFF -DOSMSCOUT_BUILD_IMPORT=OFF -DOSMSCOUT_BUILD_CLIENT_QT=OFF -DOSMSCOUT_BUILD_TOOL_OSMSCOUT2=OFF -DOSMSCOUT_BUILD_TESTS=OFF -DCMAKE_CXX_FLAGS="-Wno-psabi" -Wno-dev
else
    cmake .. -DCMAKE_BUILD_TYPE=Debug
fi
cmake --build . -j $(nproc)
sudo cmake --build . --target install
cd ..

echo buildding libosmscout release
DIRECTORY="buildrelease"
if [ ! -d "$DIRECTORY" ]; then
	mkdir $DIRECTORY
fi
cd $DIRECTORY
rm CMakeCache.txt
if [ "$rpiversion" == "raspberrypi,4" ] ; then
	cmake .. -DCMAKE_BUILD_TYPE=Release -DOSMSCOUT_ENABLE_SSE=ON -DOSMSCOUT_BUILD_DOC_API=OFF -DOSMSCOUT_BUILD_DEMOS=OFF -DOSMSCOUT_BUILD_TOOL_OSMSCOUTOPENGL=OFF -DOSMSCOUT_BUILD_IMPORT=OFF -DOSMSCOUT_BUILD_CLIENT_QT=OFF -DOSMSCOUT_BUILD_TOOL_OSMSCOUT2=OFF -DOSMSCOUT_BUILD_TESTS=OFF -DCMAKE_CXX_FLAGS="-Wno-psabi" -Wno-dev
elif [ "$rpiversion" == "raspberrypi,3" ] ; then
    cmake .. -DCMAKE_BUILD_TYPE=Release -DOSMSCOUT_ENABLE_SSE=ON -DOSMSCOUT_BUILD_DOC_API=OFF -DOSMSCOUT_BUILD_DEMOS=OFF -DOSMSCOUT_BUILD_TOOL_OSMSCOUTOPENGL=OFF -DOSMSCOUT_BUILD_IMPORT=OFF -DOSMSCOUT_BUILD_CLIENT_QT=OFF -DOSMSCOUT_BUILD_TOOL_OSMSCOUT2=OFF -DOSMSCOUT_BUILD_TESTS=OFF -DCMAKE_CXX_FLAGS="-Wno-psabi" -Wno-dev
else
    cmake .. -DCMAKE_BUILD_TYPE=Release
fi
cmake --build . -j $(nproc)
sudo cmake --build . --target install
cd ..
cd ..

echo building CarNiNe
echo "we are here"
pwd

DIRECTORY="carnine"
if [ ! -d "$DIRECTORY" ]; then
	git clone https://github.com/DerKleinePunk/carnine.git
	exitCode=$?
	if [ $exitCode -ne 0 ] ; then
	   echo "git give an Error"
	   exit $exitCode
	fi
	cd $DIRECTORY
	git submodule init
	exitCode=$?
	if [ $exitCode -ne 0 ] ; then
	   echo "git give an Error"
	   exit $exitCode
	fi
else
	cd $DIRECTORY
	if [ "$reproBuild" = "false" ] ; then
		git pull
		exitCode=$?
		if [ $exitCode -ne 0 ] ; then
			echo "git give an Error"
			exit $exitCode
		fi
	fi
	if [ ! -f modules/SDL2GuiHelper/LICENSE ]; then
		git submodule init
		exitCode=$?
		if [ $exitCode -ne 0 ] ; then
	   		echo "git give an Error"
	   		exit $exitCode
		fi
	fi
	git submodule update
	exitCode=$?
	if [ $exitCode -ne 0 ] ; then
		echo "git give an Error"
		exit $exitCode
	fi
fi

DIRECTORY="buildrelease"
if [ ! -d "$DIRECTORY" ]; then
	mkdir $DIRECTORY
fi
cd $DIRECTORY

cmake .. -DCMAKE_BUILD_TYPE=Release -DTARGET=Linux -DENABLE_CPPCHECK=OFF
exitCode=$?
if [ $exitCode -ne 0 ] ; then
	echo "cmake give an Error"
	exit $exitCode
fi

cmake --build . -j $(nproc)
exitCode=$?
if [ $exitCode -ne 0 ] ; then
	echo "cmake give an Error"
	exit $exitCode
fi
cd ..

echo Build Ok Now wie need Start/Update Service and UI

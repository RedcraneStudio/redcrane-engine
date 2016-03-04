Red Crane Engine
----------------
An engine designed  and created by your nemesis from New York, Luke San Antonio-Bialecki.

Setup
-----
- Windows
    - currently not supported
- Mac
    - currently not supported
- Linux
    - **Arch Linux** - refer to pacman for dependencies
    - **Ubuntu/Debian** - see below: *Installing dependencies for Debian/Ubuntu*

Building
--------
    mkdir build engine
    cd engine
    git clone https://bitbucket.org/lukesanantonio/redcrane-engine.git
    cd ../build
    cmake ../engine/redcrane-engine
    sudo make

Running the game
----------------
Go to source directory in the build folder

```
./redc --out-log-level=0 -config=../survival/cfg.ini
```

Installing dependencies for Debian/Ubuntu
-----------------------------------------
Use the following command:
```
sudo apt-get install libpng-dev libmsgpack-dev libluajit-5.1-dev
g++ gcc libenet-dev libsdl2-dev libfreetype6-dev libbullet-dev
```

You must also build the following libraries from source as they aren't supported
in the current iteration of the aptitude package manager.

* cmake: https://cmake.org/files/v3.5/cmake-3.5.0-rc3-Linux-x86_64.tar.gz
* libuv-dev: http://dist.libuv.org/dist/v1.8.0/
* boost: http://sourceforge.net/projects/boost/files/boost/1.60.0/
* libglm-dev: http://glm.g-truc.net/0.9.7/index.html

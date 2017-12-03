Acording to follow sequence to compile and install packages. SOURCEDIR is every source directory.

(1)zlib compile
cd SOURCEDIR
make && sudo make install

(2)jpeg compile
cd SOURCEDIR 
./configure &&make && sudo make install

(3)libxml2 compile
cd SOURCEDIR
./configure &&make && sudo make install

compile error and solution：
Question：
/usr/include/bits/fcntl2.h:51: error: call to '__open_missing_mode' declared with attribute error: open with O_CREAT in second argument needs 3 arguments
make[2]: *** [nanohttp.lo] 错误 1
Solve Method：
Modify nanohttp.c中的fd = open(filename, O_CREAT | O_WRONLY); to fd = open(filename, O_CREAT | O_WRONLY,0777);， recompile it。

(4)freetype compile
cd SOURCEDIR
./configure &&make && sudo make install

(5)libpng compile
cd SOURDIR
./autogen.sh && ./configure && make && sudo make install 

(6)tslib compile
cd SOURCEDIR
./autogen.sh && ./configure && make && sudo make install

(7)MiniGUI compile
cd SOURCEDIR
mkdir build_cmake && cd build_cmake
cmake .. && make && sudo make install 

(8)MiniGUI-res compile
cd SOURCEDIR
./autogen.sh && ./configure && make && sudo make install

(9)mgplus compile
cd SOURCEDIR
mkdir build_cmake && cd build_cmake
cmake .. && make && sudo make install 

(10)mgutils compile
cd SOURCEDIR
mkdir build_cmake && cd build_cmake
cmake .. && make && sudo make install 

(11)mgncs compile
cd SOURCEDIR
mkdir build_cmake && cd build_cmake
cmake .. && make && sudo make install 

(12)mgeff compile
cd SOURCEDIR
mkdir build_cmake && cd build_cmake
cmake .. && make && sudo make install 

(13)mgncs4pad compile
cd SOURCEDIR
mkdir build_cmake && cd build_cmake
cmake .. && make && sudo make install 

(14)libpciaccess compile
cd SOURCEDIR
./configure && make && sudo make install

(15)gvfb compile
	cd SOURCEDIR
	mkdir build_cmake && cd build_cmake
	cmake .. && make && sudo make install 

(16)uxdemo compile

chipmunk compile
	cd SOURCEDIR
	mkdir build_cmake && cd build_cmake
	cmake .. && make && sudo make install 
sqlite compile
	cd SOURCEDIR
	mkdir build_cmake && cd build_cmake
	cmake .. && make && sudo make install 
uxdemo compile
	cd SOURCEDIR
	mkdir build_cmake && cd build_cmake
	cmake .. && make && sudo make install 　


(17)uxdemo run
cd /usr/local/bin
./run_cellphone.sh


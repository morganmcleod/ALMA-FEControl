#/bin/sh

# Install GCC
pacman -S mingw-w64-i686-gcc --needed --noconfirm
# Install autotools
pacman -S mingw-w64-i686-autotools --needed --noconfirm
# Install make command
pacman -S mingw-w64-i686-make --needed --noconfirm
# Install CppUnit
pacman -S mingw-w64-i686-cppunit --needed --noconfirm
# Install expat
pacman -S mingw-w64-i686-expat --needed --noconfirm
# Install boost
pacman -S mingw-w64-i686-boost --needed --noconfirm
# Install drmingw
pacman -S mingw-w64-i686-drmingw --needed --noconfirm
# Install git
pacman -S git --needed --noconfirm

# Make a symbolic link for the make command
ln -sf $(which mingw32-make.exe) /mingw32/bin/make.exe

# ---------------
# Install expatmm
# ---------------
echo "------------------"
echo "Installing expatmm"
echo "------------------"
cd ~
git clone https://github.com/ckane/expatmm
cd expatmm
# Remove this lane. It fails for newer GCC versions
sed -i -e "/LDFLAGS -no-undefined/d" configure.ac
# Change headers installation path
sed -i -e "s/\/expatmm-1.0//" src/Makefile.am
# Change headers installation path
sed -i -e "s/\/expatmm-1.0//" expatmm.pc.in
./autogen.sh
./configure
 # Change these lines so we can build the dynamic library
sed -i -e "s/\(allow_undefined=\)yes/\1no/" libtool
# Make and install
make install
cd ~
# Remove clone
rm -rf ./expatmm

echo "-----"
echo "Done!"
echo "-----"
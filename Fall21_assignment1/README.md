##How to run
Run in MacOS X
1) Install XCode
2) Install [Xquartz](https://www.xquartz.org/releases/XQuartz-2.8.1.html)
3) After you installed the latest Xquartz and restarted the computer,
if getting `fatal error: 'X11/Xlib.h' file not found` after installing Xquartz, you may want to add a symlink to your X11 installation folder by running the following command in the terminal:
```shell
ln -s /opt/X11/include/X11 /usr/local/include/X11
```
4) If getting `fatal error: 'tiffio.h' file not found`, You may resolve this by installing libtiff via homebrew
```shell
brew install libtiff
```
5) Compile the pic library before compiling the starter code
```shell
> cd pic
> make
> cd ..
> cd assign1
> make
> ./assign1 spiral.jpg
```

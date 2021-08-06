# zMemoryPool
A memory pool for media application

### build
Build for vs2008 
```
# must define BOOST_DIR for using boost library when compile with msvc90
cmake --install-prefix D:\project\zMemoryPool\export -B build_vs2008 -G "Visual Studio 9 2008" -D BOOST_DIR=d:\3rdlib\boost-1_71
# for x64
cmake --install-prefix D:\project\zMemoryPool\export -B build_vs2008_x64 -G "Visual Studio 9 2008 Win64" -D BOOST_DIR=d:\3rdlib\boost-1_71
```

WIP
CGAL 2D Triangulations on the Sphere in Houdini
https://doc.cgal.org/latest/Triangulation_on_sphere_2/index.html#Chapter_2D_Triangulations_on_sphere

1. install cgal windows
https://doc.cgal.org/latest/Manual/windows.html
```
.\vcpkg.exe install cgal:x64-windows
```
  
2. build
in Houdini Command Line Tools
```
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=VCPKG_PATH\scripts\buildsystems\vcpkg.cmake ..
cmake --build . --clean-first
```

3. add PATH

```
__My_Houdini_User_Pref__/bin
```
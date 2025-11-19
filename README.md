# [P3] Interactive Loading Screen

## Authors
- Isaac A. Dicayanan
- Raine Miguel V. Villaver

## Setting up the project
In Visual Studio 2022:
1. Clone the project
2. Run `CMakeLists.txt`
3. Select `P3.exe` as startup item
4. Run the program

## Common Issue
- If the program fails to start due to missing `openal32.dll`:
  - Copy `openal32.dll` from this location \thirdparty\sfml-src\extlibs\bin\x64
  - Place it in the same directory as `P3.exe` located in \out\build\x64-Debug\bin

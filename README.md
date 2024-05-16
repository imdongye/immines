## immines

simple mine sweeper game and probability calculator with imgui

tested in windows11 and mac(silicon, intel)

[original repo](https://github.com/imdongye/imtoys)
[main src code](https://github.com/imdongye/immines/blob/main/main/im_mine_sweeper/app_mine_sweeper.cpp)
[document](https://imdongye.notion.site/Mine-sweeper-11d1fda8afb641349d2a951c9b85790c?pvs=4)
## Build / Install Instructions

```
brew install cmake OR install @ https://cmake.org/download/

git clone https://github.com/imdongye/immines.git
cd immines
git submodule init
git submodule update

cmake -G["Xcode","Ninja","Visual Studio 17 2022"] -Bbuild .
cmake --build build --config Debug
```

## Dependency

c++17, glfw, glad(OpenGL4.1 core), glm, imgui, ~~nlohmann/json, stb(image, write, truetype)~~
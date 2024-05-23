#[==[
  
2023-07-09 / imdongye

cmake -B build . -G [Xcode, Ninja, Visual Studio 17 2022 ...]
cmake --build build --config Debug
or
setting in vscode cmake extention

실행경로는 main 폴더, assets폴더를 사용했다면 실행파일을 배포할때 assets 폴더도 같이 배포해야한다.
assimp, glfw 의 BUILD_SHARED_LIBS 옵션을 삭제하고 현재 스코프의 BUILD_SHARED_LIBS를 참조하게 한다.

Note:
* target_link_libraries로 연결한 cmake 라이브러리(.lib나 .a를 생략해도 됨)는 target_include_directories 로 헤더 경로를 입력 안해줘도 자동으로 된다.

]==]


cmake_minimum_required(VERSION 3.24...3.27 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DLIM_DEBUG")

set(CMAKE_VERBOSE_MAKEFILE true)
if(WIN32)
    set(CMAKE_GENERATOR_PLATFORM x64)
endif()

project(
    immines
    VERSION 1.0
    DESCRIPTION "immines"
    LANGUAGES C CXX
)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    #add_compile_options(/wd4819) # 코드페이지 949 경고
    add_compile_options(/utf-8)
endif()


set(dep_libs)


### glfw ###
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/glfw)
set_target_properties(glfw PROPERTIES CONFIG Release)
target_compile_definitions(glfw INTERFACE GLFW_INCLUDE_NONE) # Todo?: glad include 순서 무시하기위함
set_target_properties(glfw PROPERTIES FOLDER "vendor")
set(dep_libs ${dep_libs} glfw)






### glad ### C
# opengl function pointers (replaced by glad)
set(lib_name "glad")
set(lib_dir "${CMAKE_SOURCE_DIR}/vendor/glad-gl4.1-core")
file(GLOB_RECURSE lib_files "${lib_dir}/*.*")
add_library(${lib_name} STATIC ${lib_files})
source_group(TREE ${lib_dir} FILES ${lib_files})
target_include_directories(${lib_name} PUBLIC "${lib_dir}/include")
set_target_properties(${lib_name} PROPERTIES FOLDER "vendor")
set(dep_libs ${dep_libs} ${lib_name})





### stb ### C
set(lib_name "stb")
set(lib_dir "${CMAKE_SOURCE_DIR}/vendor/stb")
file(GLOB_RECURSE lib_files "${lib_dir}/*.*")
add_library(${lib_name} STATIC ${lib_files})
source_group(TREE ${lib_dir} FILES ${lib_files})
target_include_directories(${lib_name} PUBLIC ${lib_dir})
set_target_properties(${lib_name} PROPERTIES FOLDER "vendor")
set(dep_libs ${dep_libs} ${lib_name})




### imgui ###
set(lib_name "imgui")
set(lib_dir "${CMAKE_SOURCE_DIR}/vendor/imgui-docking-1.90")
file(GLOB_RECURSE lib_files "${lib_dir}/*.*")
list(REMOVE_ITEM lib_files "${lib_dir}/misc/freetype/imgui_freetype.cpp")
list(REMOVE_ITEM lib_files "${lib_dir}/examples/example_glfw_opengl3/main.cpp")
list(REMOVE_ITEM lib_files "${lib_dir}/misc/freetype/imgui_freetype.h")
add_library(${lib_name} STATIC ${lib_files})
source_group(TREE ${lib_dir} FILES ${lib_files})
target_include_directories(${lib_name} PUBLIC ${lib_dir})
target_link_libraries(${lib_name} PUBLIC glfw stb)
add_dependencies(${lib_name} glfw stb)
target_compile_definitions(${lib_name}
    PRIVATE
        IMGUI_DISABLE_STB_TRUETYPE_IMPLEMENTATION
        IMGUI_STB_TRUETYPE_FILENAME="../stb/stb_truetype.h"
        IMGUI_DISABLE_STB_RECT_PACK_IMPLEMENTATION
        IMGUI_STB_RECT_PACK_FILENAME="../stb/stb_rect_pack.h"
)
set_target_properties(${lib_name} PROPERTIES FOLDER "vendor")
set(dep_libs ${dep_libs} ${lib_name})





### glm (header only) ###
set(lib_name "glm")
set(lib_dir "${CMAKE_SOURCE_DIR}/vendor/glm-0.9.9.8")
file(GLOB_RECURSE lib_files "${lib_dir}/*.*")
add_library(${lib_name} INTERFACE ${lib_files})
source_group(TREE "${lib_dir}/glm" FILES ${lib_files})
target_include_directories(${lib_name} INTERFACE ${lib_dir})
target_compile_definitions(${lib_name} INTERFACE GLM_FORCE_SILENT_WARNINGS)
set_target_properties(${lib_name} PROPERTIES FOLDER "vendor")
set(dep_libs ${dep_libs} ${lib_name})






### json (header only) ###
set(lib_name "json")
set(lib_dir "${CMAKE_SOURCE_DIR}/vendor/json")
file(GLOB_RECURSE lib_files "${lib_dir}/*.*")
add_library(${lib_name} INTERFACE ${lib_files})
source_group(TREE ${lib_dir} FILES ${lib_files})
target_include_directories(${lib_name} INTERFACE ${lib_dir})
set_target_properties(${lib_name} PROPERTIES FOLDER "vendor")
set(dep_libs ${dep_libs} ${lib_name})




### limbrary ###
# 배포할땐 cpp를 삭제하고 배포하기로 하자.
set(lib_name "limbrary")
set(lib_dir "${CMAKE_SOURCE_DIR}/limbrary")
file(GLOB_RECURSE lib_files CONFIGURE_DEPENDS "${lib_dir}/*.*")
add_library(${lib_name} STATIC ${lib_files})
source_group(TREE "${lib_dir}" FILES ${lib_files})
target_include_directories(${lib_name} PUBLIC ${lib_dir})
target_link_libraries(${lib_name} PUBLIC ${dep_libs})
add_dependencies(${lib_name} ${dep_libs})
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_definitions(${lib_name}
        PUBLIC
            _SCL_SECURE_NO_WARNINGS  # Calling any one of the potentially unsafe methods in the Standard C++ Library
            _CRT_SECURE_NO_WARNINGS  # Calling any one of the potentially unsafe methods in the CRT Library
    )
    target_compile_options(${lib_name} 
        PUBLIC
            /MD$<$<CONFIG:Debug>:d>
        PRIVATE
            /W4
            /wd4100 # 참조되지않은 매개변수
            /wd4244 # double to float
            /wd4267 # size_t to uint
            /wd4018 # < : signed, unsinged
            /wd4702 # program.cpp 접근할수없는 코드
            #/WX
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_definitions(${lib_name} 
        PUBLIC
            GL_SILENCE_DEPRECATION
    )
    target_compile_options(${lib_name} 
        PRIVATE
            -Wall
            #-Werror
    )
endif()




### main proj ###
set(exe_name "immines")
set(main_dir "${CMAKE_SOURCE_DIR}/main")
file(GLOB_RECURSE main_files   CONFIGURE_DEPENDS 
    ${main_dir}/*.c ${main_dir}/*.cpp ${main_dir}/*.h ${main_dir}/*.hpp ${main_dir}/*.ipp
)
file(GLOB_RECURSE shader_files CONFIGURE_DEPENDS 
    ${main_dir}/*.glsl ${main_dir}/*.vs ${main_dir}/*.vert ${main_dir}/*.fs ${main_dir}/*.frag
)
list(FILTER main_files   EXCLUDE REGEX "${main_dir}/ignored/*")
list(FILTER shader_files EXCLUDE REGEX "${main_dir}/ignored/*")

add_executable(${exe_name} WIN32 MACOSX_BUNDLE ${main_files} ${shader_files})
if(WIN32)
    set_target_properties(${exe_name} PROPERTIES 
        LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")
endif(WIN32)
source_group(TREE ${main_dir} FILES ${main_files})
target_include_directories(${exe_name} PRIVATE ${main_dir})
target_link_libraries(${exe_name} PRIVATE limbrary)
add_dependencies(${exe_name} limbrary)
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(
        ${exe_name} 
        PRIVATE
            /W4
            #/WX
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(
        ${exe_name} 
        PRIVATE
            -Wall
            #-Werror
    )
endif()
target_compile_definitions(
    ${exe_name} 
    PRIVATE
        WINDOW_NAME="imtoys template"
        WINDOW_WIDTH=600
        WINDOW_HEIGHT=400
)




### 결과 파일 생성 위치 지정 ###
set_target_properties(limbrary ${dep_libs} ${exe_name} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/library) # static, dll load lib
set_target_properties(limbrary ${dep_libs} ${exe_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/module) # dll of module 
set_target_properties(limbrary ${dep_libs} ${exe_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/executable) # exe






set(working_dir "${CMAKE_SOURCE_DIR}/main")

if(CMAKE_GENERATOR MATCHES "Visual Studio*")
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${exe_name})
    # From: https://stackoverflow.com/questions/41864259/how-to-set-working-directory-for-visual-studio-2017-rc-cmake-project
    set_target_properties(${exe_name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${working_dir})
endif()
if(CMAKE_GENERATOR MATCHES "Xcode")
    set_target_properties(${exe_name} PROPERTIES 
                    XCODE_GENERATE_SCHEME TRUE
                    XCODE_SCHEME_WORKING_DIRECTORY ${working_dir})
    # 시작 schema 설정 못찾음
endif()



### CPack ###
# From: https://cmake.org/cmake/help/book/mastering-cmake/chapter/Packaging%20With%20CPack.html
# And : https://martin-fieber.de/blog/cmake-cpack-cross-platform-distributables/#windows-application-bundle
set(CPACK_VERBATIM_VARIABLES YES)
set(CPACK_PACKAGE_VENDOR "idy soft")
set(CPACK_PACKAGE_DIRECTORY distribution)
set(CPACK_SOURCE_PACKAGE_FILE_NAME "iimmm-${CMAKE_PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})

if(WIN32)
    install(TARGETS ${exe_name} DESTINATION .)
    install(DIRECTORY ${main_dir}/assets/ DESTINATION ./assets)
    install(FILES ${main_dir}/imgui.ini DESTINATION ./)

    target_sources(${exe_name} PUBLIC
        main/assets/manifests/main.rc
        main/assets/manifests/app.manifest
    )
    set(CPACK_NSIS_DISPLAY_NAME "immines")
    set(CPACK_NSIS_PACKAGE_NAME "immines")
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_MANIFEST_DPI_AWARE true)
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL YES)
    set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/imdongye/immines")
    set(CPACK_NSIS_HELP_LINK "http://imdongye.duckdns.org/")
    set(CPACK_NSIS_CONTACT "imdongye@naver.com")

    set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/main\\\\assets\\\\icons\\\\icon.ico")
    set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/main\\\\assets\\\\icons\\\\icon.ico")
    set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/main\\\\assets\\\\icons\\\\icon.ico")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/License.txt")
    # From: https://stackoverflow.com/questions/2640760/creating-windows-desktop-icon-in-cmake-cpack-nsis
    list(APPEND CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut '$DESKTOP\\\\아임마인즈.lnk' '$INSTDIR\\\\immines.exe'")
    list(APPEND CPACK_NSIS_DELETE_ICONS_EXTRA "Delete '$DESKTOP\\\\${CMAKE_PROJECT_NAME}.lnk'")

    list(APPEND CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\아임마인즈.lnk' '$INSTDIR\\\\immines.exe'")
    list(APPEND CPACK_NSIS_DELETE_ICONS_EXTRA "Delete '$SMPROGRAMS\\\\$START_MENU\\\\${CMAKE_PROJECT_NAME}.lnk'")



elseif(APPLE)
    install(TARGETS ${exe_name} 
        BUNDLE  DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    set(mac_bundle_resource
        "main/assets/manifests/Info.plist"
        "main/assets/icons/icon.icns"
        "main/imgui.ini"
        "main/assets/fonts/fontello.ttf"
        "main/assets/fonts/SpoqaHanSansNeo-Medium.ttf"
    )
    target_sources(${exe_name} PUBLIC
        ${mac_bundle_resource}
    )
    source_group(TREE "${CMAKE_SOURCE_DIR}/main" FILES ${mac_bundle_resource})
    # icon
    set_source_files_properties("main/assets/icons/icon.icns"
        PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
    )
    # imgui.ini
    set_source_files_properties("main/imgui.ini"
        PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
    )
    # fonts
    set_source_files_properties("main/assets/fonts/fontello.ttf"
        PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources/assets/fonts"
    )
    set_source_files_properties("main/assets/fonts/SpoqaHanSansNeo-Medium.ttf"
        PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources/assets/fonts"
    )

    set_target_properties(${exe_name} PROPERTIES
        # INSTALL_RPATH @executable_path/../Frameworks
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/main/assets/manifests/Info.plist"
        MACOSX_BUNDLE_BUNDLE_VERSION "${BUILD_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.idysoft.immines"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.idysoft.immines"
        MACOSX_BUNDLE_COPYRIGHT "(c) 2024 idy soft. All rights reserved."    
    )
    set(XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "7T6AP7F7YU")
    set(XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "Apple Development")
    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Manual")
    set(CPACK_GENERATOR "ZIP")
endif()

include(CPack)
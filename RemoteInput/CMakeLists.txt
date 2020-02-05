cmake_minimum_required(VERSION 3.15)
project(RemoteInput VERSION 1.0.0 DESCRIPTION "Remote Input")

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(Echo)
include_directories(Hooks)
include_directories(Java)
include_directories(Platform)
include_directories(Plugin)


# PACKAGES
#find_package(Java 1.8 REQUIRED)
find_package(JNI 1.8 REQUIRED)
include_directories(${JNI_INCLUDE_DIRS})


# DIRECTORIES
IF(WIN32)
    include_directories(Thirdparty/min_hook/include)
    include_directories(Thirdparty/min_hook/src)
    include_directories(Thirdparty/min_hook/src/hde)
ELSEIF(APPLE)
    include_directories(Thirdparty/rd_route/include)
ELSE()
ENDIF()


# INCLUDES
IF(WIN32)
    set(SRC_LIST
            Thirdparty/min_hook/include/MinHook.h
            Thirdparty/min_hook/src/hde/hde32.c
            Thirdparty/min_hook/src/hde/hde32.h
            Thirdparty/min_hook/src/hde/hde64.c
            Thirdparty/min_hook/src/hde/hde64.h
            Thirdparty/min_hook/src/hde/pstdint.h
            Thirdparty/min_hook/src/hde/table32.h
            Thirdparty/min_hook/src/hde/table64.h
            Thirdparty/min_hook/src/buffer.c
            Thirdparty/min_hook/src/buffer.h
            Thirdparty/min_hook/src/hook.c
            Thirdparty/min_hook/src/trampoline.c
            Thirdparty/min_hook/src/trampoline.h)
ELSEIF(APPLE)
    set(SRC_LIST
            Thirdparty/rd_route/include/rd_route.h
            Thirdparty/rd_route/src/rd_route.c)
ELSE()
ENDIF()


# PLATFORM
IF(WIN32)
ELSEIF(APPLE)
    find_path(FOUNDATION_INCLUDES Foundation/Foundation.h)
    find_path(COCOA_INCLUDES Cocoa/Cocoa.h)
    find_path(OPENGL_INCLUDES OpenGL/OpenGL.h)

    set(EXTRA_INCLUDES
            ${FOUNDATION_INCLUDES}
            ${COCOA_INCLUDES}
            ${OPENGL_INCLUDES})
ELSE()
ENDIF()

IF(WIN32)
    set(EXTRA_LIBRARIES
            OpenGL32)
ELSEIF(APPLE)
    find_library(FOUNDATION Foundation)
    find_library(COCOA Cocoa)
    find_library(OPENGL OpenGL)

    set(EXTRA_LIBRARIES
            ${FOUNDATION}
            ${COCOA}
            ${OPENGL})
ELSE()
ENDIF()

# LIBRARIES
IF(WIN32)
    set(LIBRARIES_LIST
            ${JAVA_AWT_LIBRARY})
ELSEIF(APPLE)
    set(JAVA_JNI_LIBRARY_PATH
            /Library/Internet\ Plug-Ins/JavaAppletPlugin.plugin/Contents/Home/lib)

    set(JAVA_JNI_LIBRARY_RPATH
            "/Library/Internet Plug-Ins/JavaAppletPlugin.plugin/Contents/Home/lib")

    find_library(Java_JAWT_LIBRARY
            NAMES libawt.dylib
            HIMES ${_JAVA_HINTS}
            PATHS ${JAVA_JNI_LIBRARY_PATH})

    find_library(Java_JAWT_EXTENDED_LIBRARY
            NAMES libawt_lwawt.dylib
            HIMES ${_JAVA_HINTS}
            PATHS ${JAVA_JNI_LIBRARY_PATH})

    set(OTHER_LDFLAGS
            ${OTHER_LDFLAGS}
            "-rpath \"${JAVA_JNI_LIBRARY_RPATH}\""
            "-rpath \"${JAVA_JNI_LIBRARY_RPATH}/server\"")

    set(LIBRARIES_LIST
            ${Java_JAWT_LIBRARY}
            ${Java_JAWT_EXTENDED_LIBRARY}
            ${OTHER_LDFLAGS})
ELSE()
    find_library(Java_JAWT_LIBRARY
            NAMES libawt.so
            HIMES ${_JAVA_HINTS}
            PATHS ${_JAVA_HOME}/lib)

    find_library(Java_JAWT_EXTENDED_LIBRARY
            NAMES libawt_lwawt.so
            HIMES ${_JAVA_HINTS}
            PATHS ${_JAVA_HOME}/lib)

    set(LIBRARIES_LIST
            ${Java_JAWT_LIBRARY}
            ${Java_JAWT_EXTENDED_LIBRARY})
ENDIF()


# SOURCES
set(SRC_LIST
        ${SRC_LIST}
        ${EXTRA_INCLUDES})

set(SRC_LIST
        ${SRC_LIST}
        ${PROJECT_NAME}
        Echo/MemoryMap.hxx
        Echo/Module.cxx
        Echo/Module.hxx
        Echo/SharedEvent.cxx
        Echo/SharedEvent.hxx
        #Echo/SpinLock.cxx
        #Echo/SpinLock.hxx
        Echo/Time.cxx
        Echo/Time.hxx
        Hooks/Hooks.cpp
        Hooks/Hooks.hpp
        #Hooks/ModelRendering.cpp
        #Hooks/ModelRendering.hpp
        Java/Applet.cxx
        Java/Applet.hxx
        Java/AWTAccessor.cxx
        Java/AWTAccessor.hxx
        Java/AWTEvent.cxx
        Java/AWTEvent.hxx
        Java/AWTEventAccessor.cxx
        Java/AWTEventAccessor.hxx
        Java/Component.cxx
        Java/Component.hxx
        Java/EventQueue.cxx
        Java/EventQueue.hxx
        Java/FocusEvent.cxx
        Java/FocusEvent.hxx
        Java/Frame.cxx
        Java/Frame.hxx
        Java/InputEvent.cxx
        Java/InputEvent.hxx
        Java/KeyEvent.cxx
        Java/KeyEvent.hxx
        Java/MouseEvent.cxx
        Java/MouseEvent.hxx
        Java/MouseWheelEvent.cxx
        Java/MouseWheelEvent.hxx
        Java/Toolkit.cxx
        Java/Toolkit.hxx
        Platform/DebugConsole.cxx
        Platform/DebugConsole.hxx
        Platform/JavaInternal.hxx
        Platform/NativeHooks.hxx
        Platform/NativeHooks_Darwin.cxx
        Platform/NativeHooks_Linux.cxx
        Platform/NativeHooks_Windows.cxx
        Platform/Platform.hxx
        #Platform/Platform_Darwin.mm
        Platform/Platform_Linux.cxx
        Platform/Platform_Windows.cxx
        Plugin/ControlCenter.cxx
        Plugin/ControlCenter.hxx
        Plugin/Graphics.cxx
        Plugin/Graphics.hxx
        Plugin/InputOutput.cxx
        Plugin/InputOutput.hxx
        Plugin/Plugin.cxx
        Plugin/Plugin.hxx
        Plugin/Signal.hxx
        Plugin/SimbaPlugin.cxx
        Plugin/SimbaPlugin.hxx
        Plugin/TMemoryManager.hxx
        DetachedThreadPool.cxx
        DetachedThreadPool.hxx
        EIOS.cxx
        EIOS.hxx
        JVM.cxx
        JVM.hxx
        Random.cxx
        Random.hxx
        Reflection.cxx
        Reflection.hxx
        ReflectionHook.cxx
        ReflectionHook.hxx
        RemoteInput.h
        ThreadPool.cxx
        ThreadPool.hxx)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

set(CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} -g -static -static-libgcc -static-libstdc++")

set(CMAKE_CXX_FLAGS_RELEASE
        "${CMAKE_CXX_FLAGS_RELEASE} -O3 -s -static -static-libgcc -static-libstdc++")

IF(APPLE)
    set(SRC_LIST
            ${SRC_LIST}
            Platform/Platform_Darwin.mm)
ENDIF()

set(LIBRARIES_LIST
        ${LIBRARIES_LIST}
        ${EXTRA_LIBRARIES})

add_library(${PROJECT_NAME} SHARED
        ${SRC_LIST})
target_link_libraries(${PROJECT_NAME}
        ${LIBRARIES_LIST})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
# Fixing libpqxx Double-Free Error on Linux

## Problem Description

When exiting a C++ application using libpqxx (PostgreSQL C++ library), the following error occurs:

```
double free or corruption (!prev)
[1]    12345 IOT instruction (core dumped)  ./app
```

## Root Cause

The double-free error is caused by linking against libpqxx as a **shared library** (`.so` files) with C++ ABI incompatibilities between how the library was compiled and how the application is compiled. This is a **known issue** with libpqxx on Linux, particularly with GCC.

From the libpqxx GitHub issue #681:
> "I have reproduced the problem. It only happens when linking dynamically. On non-Windows systems I don't recommend using libpqxx as a shared library in any case; C++ ABIs are just too detailed to make backward binary compatibility very useful."

The solution is to **build and link libpqxx statically** instead of using the shared library.

## Solution: Static Linking

### Step 1: Build libpqxx as a Static Library

```bash
# Install build dependencies
sudo pacman -S postgresql-libs cmake base-devel

# Download libpqxx source (version 7.10.1 in this example)
cd /tmp
wget https://github.com/jtv/libpqxx/archive/refs/tags/7.10.1.tar.gz
tar -xzf 7.10.1.tar.gz
cd libpqxx-7.10.1

# Create build directory and configure for STATIC library only
mkdir build && cd build
cmake .. \
  -DCMAKE_INSTALL_PREFIX=/usr/local \
  -DBUILD_SHARED_LIBS=OFF \
  -DCMAKE_BUILD_TYPE=Release

# Build (use all CPU cores)
make -j$(nproc)

# Install
sudo make install

# Verify the static library was installed
ls -lh /usr/local/lib/libpqxx.a
```

Expected output:
```
-rw-r--r-- 1 root root 14M Dec 19 20:03 /usr/local/lib/libpqxx.a
```

### Step 2: Update Build Configuration

For a project using **Premake5** with the following structure:
```
finances/
├── app/
│   ├── source/
│   └── Build-App.lua
├── core/
│   ├── source/
│   └── Build-Core.lua
├── cli/
│   ├── source/
│   └── Build-Cli.lua
└── Build.lua
```

#### Update `Build-Core.lua` (Static Library)

```lua
project("core")
    kind("StaticLib")
    language("C++")
    cppdialect("C++20")
    targetdir("binaries/%{cfg.buildcfg}")
    staticruntime("off")
    
    files({ "source/**.h", "source/**.cpp" })
    
    includedirs({
        "source",
        "/usr/local/include",  -- Add this for static libpqxx headers
    })
    
    -- REMOVE the links block entirely
    -- Static libraries don't need to link dependencies
    -- The final executable will do the linking
    
    targetdir("../binaries/" .. OutputDir .. "/%{prj.name}")
    objdir("../binaries/intermediates/" .. OutputDir .. "/%{prj.name}")
    
    filter("system:windows")
        systemversion("latest")
        defines({})
    
    filter("configurations:debug")
        defines({ "DEBUG" })
        runtime("debug")
        symbols("On")
    
    filter("configurations:release")
        defines({ "RELEASE" })
        runtime("release")
        optimize("On")
        symbols("On")
    
    filter("configurations:dist")
        defines({ "DIST" })
        runtime("release")
        optimize("On")
        symbols("Off")
```

#### Update `Build-App.lua` (Executable)

```lua
project("app")
    kind("ConsoleApp")
    language("C++")
    cppdialect("C++20")
    targetdir("binaries/%{cfg.buildcfg}")
    staticruntime("off")
    
    files({ "source/**.h", "source/**.cpp" })
    
    includedirs({
        "source",
        "../core/source",
        "../cli/source",
        "/usr/local/include",  -- Add this for static libpqxx headers
    })
    
    -- Use --whole-archive to force inclusion of all libpqxx symbols
    -- This ensures all symbols are available when core.a needs them
    linkoptions({
        "-Wl,--whole-archive",
        "/usr/local/lib/libpqxx.a",
        "-Wl,--no-whole-archive",
    })
    
    -- Link your own libraries and dependencies
    links({
        "core",
        "cli",
        "pq",        -- PostgreSQL client library
        "pthread",   -- Required by libpqxx
    })
    
    targetdir("../binaries/" .. OutputDir .. "/%{prj.name}")
    objdir("../binaries/intermediates/" .. OutputDir .. "/%{prj.name}")
    
    filter("system:windows")
        systemversion("latest")
        defines({ "WINDOWS" })
    
    filter("configurations:debug")
        defines({ "DEBUG" })
        runtime("debug")
        symbols("On")
    
    filter("configurations:release")
        defines({ "RELEASE" })
        runtime("release")
        optimize("On")
        symbols("On")
    
    filter("configurations:dist")
        defines({ "DIST" })
        runtime("release")
        optimize("On")
        symbols("Off")
```

### Step 3: Rebuild Your Project

```bash
# Navigate to your project directory
cd ~/path/to/your/project

# Regenerate build files
premake5 gmake2

# Clean previous build
make clean

# Rebuild
make config=debug

# Verify static linking (should show NO libpqxx.so)
ldd ./binaries/linux-x86_64/debug/app/app | grep pqxx
```

**Expected result:** The `ldd` command should show **no output** for pqxx, confirming static linking.

## Key Technical Points

### Why `--whole-archive` is Necessary

The `--whole-archive` linker flag forces the linker to include **all** symbols from `libpqxx.a`, not just the ones that are currently needed. This is crucial because:

1. Your `core` library uses libpqxx symbols
2. Without `--whole-archive`, the linker processes libraries left-to-right
3. When linking `core.a`, if libpqxx symbols aren't already available, you get "undefined reference" errors
4. `--whole-archive` ensures all libpqxx symbols are loaded upfront

### Link Order Matters

The correct link order is:
1. `--whole-archive` + static libpqxx
2. Your application libraries (core, cli)
3. System libraries (pq, pthread)

### Why Static Linking Fixes the Issue

- **Static linking** embeds the libpqxx code directly into your binary
- The code is compiled with the **same compiler settings** as your application
- This avoids C++ ABI mismatches (name mangling, vtable layouts, exception handling)
- Destructors are called correctly, preventing double-free errors

## Verification Commands

```bash
# 1. Check that static library exists
ls -lh /usr/local/lib/libpqxx.a

# 2. Verify symbols are in the library
nm /usr/local/lib/libpqxx.a | c++filt | grep "transaction_base::commit"

# 3. Confirm your binary is NOT using shared libpqxx
ldd ./binaries/linux-x86_64/debug/app/app | grep pqxx
# Should return NO OUTPUT

# 4. Run your application and test exit (option 7)
./binaries/linux-x86_64/debug/app/app
```

## Alternative Build Systems

### For CMake

```cmake
# Find PostgreSQL
find_package(PostgreSQL REQUIRED)

# Your executable
add_executable(app main.cpp database.cpp)

# Include directories
target_include_directories(app PRIVATE 
    ${CMAKE_SOURCE_DIR}
    /usr/local/include
    ${PostgreSQL_INCLUDE_DIRS}
)

# Link with whole-archive for static libpqxx
target_link_libraries(app PRIVATE
    -Wl,--whole-archive
    /usr/local/lib/libpqxx.a
    -Wl,--no-whole-archive
    ${PostgreSQL_LIBRARIES}
    pthread
)
```

### For Makefile

```makefile
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I. -I/usr/local/include
LDFLAGS = -L/usr/local/lib
LIBS = -Wl,--whole-archive /usr/local/lib/libpqxx.a -Wl,--no-whole-archive -lpq -lpthread

SOURCES = main.cpp database.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = app

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
```

## Troubleshooting

### Issue: "undefined reference to pqxx symbols"

**Cause:** Link order is incorrect or `--whole-archive` is missing.

**Solution:** Ensure libpqxx.a comes before your libraries that use it, and use `--whole-archive`.

### Issue: "cannot find -llibpqxx.a"

**Cause:** Premake is adding `-l` prefix to the full path.

**Solution:** Use `linkoptions()` instead of `links()` for full paths.

### Issue: Still getting double-free error

**Cause:** Still linking against shared library.

**Solution:** 
```bash
# Verify you're NOT using shared library
ldd ./your_binary | grep pqxx

# If you see libpqxx.so, you're still linking dynamically
# Make sure /usr/local/lib/libpqxx.a exists and check your build config
```

## Summary

The double-free error with libpqxx is caused by C++ ABI incompatibilities when using shared libraries. The fix involves:

1. ✅ Building libpqxx as a **static library** (`-DBUILD_SHARED_LIBS=OFF`)
2. ✅ Adding `/usr/local/include` to include directories
3. ✅ Using `--whole-archive` flag to include all libpqxx symbols upfront
4. ✅ Linking with full path: `/usr/local/lib/libpqxx.a`
5. ✅ Adding required dependencies: `-lpq -lpthread`
6. ✅ Removing `links({ "pqxx" })` which finds the shared library

After these changes, the application will use the statically-linked libpqxx and the double-free error will be eliminated.

## References

- [libpqxx GitHub Issue #681](https://github.com/jtv/libpqxx/issues/681)
- [libpqxx Documentation](https://libpqxx.readthedocs.io/)
- Author recommendation: "On non-Windows systems I don't recommend using libpqxx as a shared library"

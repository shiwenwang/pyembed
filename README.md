# PyEmbed
A very simple wrapper for embed python in C++11.

# Configuration
## Windows
    - `set PYTHONHOME = ${YOUR_PYTHON_HOME_DIR}`
    - specify PYTHON_MAJOR and PYTHON_MINOR in CMakeLists.txt

## Linux
    - `export PYTHONHOME = ${YOUR_PYTHON_HOME_DIR}`
    - specify PYTHON_MAJOR and PYTHON_MINOR in CMakeLists.txt

# Build
```bash
mkdir build && cd build
cmake ..
cmake --build . --config Realse
```

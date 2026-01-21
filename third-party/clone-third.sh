#!/bin/bash

echo "clone yoga, C++ 20, v3.2.1"
git clone -b v3.2.1 --depth=1 https://github.com/facebook/yoga.git
echo "delete 'add_subdirectory(tests)' in yoga/CMakeLists.txt to disable tests"



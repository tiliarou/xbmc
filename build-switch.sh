rm -rf kodi-switch-build
mkdir kodi-switch-build
pushd kodi-switch-build
CMAKE_EXECUTABLE="/programming/kodi/tools/depends/native/cmake-native/x86_64-linux-native/bin/cmake"

${CMAKE_EXECUTABLE} -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=/programming/kodi/tools/depends/target/Toolchain.cmake \
                          -DENABLE_PYTHON=OFF \
                          -DENABLE_AIRTUNES=OFF \
                          -ENABLE_X=OFF \
                          ../kodi
popd

all:
	g++ -std=c++17 -pthread -I . -I src/include -I src/lib/gtest/include -L src/lib -L C:/msys64/mingw64/lib -o dist/main src/main.cpp src/visualizer.cpp src/audioProcessor.cpp src/helper.cpp src/chordDictionary.cpp src/logger.cpp src/mainTest.cpp -lgtest -lgtest_main -lmingw32 -lSDL2main -lSDL2 -static-libgcc -static-libstdc++






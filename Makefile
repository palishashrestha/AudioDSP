all:
	g++ -std=c++11 -pthread -I . -I src/include -L C:/msys64/mingw64/lib -o dist/main src/main.cpp src/visualizer.cpp src/audioProcessor.cpp src/helper.cpp src/chordDictionary.cpp src/logger.cpp -lmingw32 -lSDL2main -lSDL2



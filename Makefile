all:
	g++ -I . -I src/include -L src/lib -o dist/main src/main.cpp src/visualizer.cpp src/audioProcessor.cpp src/helper.cpp src/chordDictionary.cpp -lmingw32 -lSDL2main -lSDL2


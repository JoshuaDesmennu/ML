main: main.cpp idx.cpp matrix.cpp neural.cpp
	g++ -omain main.cpp idx.cpp matrix.cpp neural.cpp graphix.cpp -lm -Wall -Werror -std=c++20 -lSDL2_ttf -lSDL2  -O3

test: model_use.cpp idx.cpp matrix.cpp neural.cpp
	g++ -otest model_use.cpp idx.cpp matrix.cpp neural.cpp -lm -Wall -Werror -std=c++20
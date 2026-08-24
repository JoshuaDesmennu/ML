main: main.cpp idx.cpp matrix.cpp neural.cpp
	g++ -omain main.cpp idx.cpp matrix.cpp neural.cpp -lm -Wall -Werror -std=c++20

test: model_use.cpp idx.cpp matrix.cpp neural.cpp
	g++ -otest model_use.cpp idx.cpp matrix.cpp neural.cpp -lm -Wall -Werror -std=c++20
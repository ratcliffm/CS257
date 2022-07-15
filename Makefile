Image-Convolution: main.cpp ppm.cpp ppm.h
	g++ -Wall -O2 -g main.cpp ppm.cpp -std=c++11 -o ./Image-Convolution

.PHONY: clean
clean:
	rm -f Image-Convolution

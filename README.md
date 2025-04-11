# Quadtree Image Compressor
This program attempts to compress images by removing redundancy by normalizing like-colored block of areas. The algorithm employs divide and conquer method by using a quadtree structure. An image is recursively divided into four equal rectangular blocks until the subblock is found to be similarly colored.

## Requirements
The program uses the following libraries
- CImg
- libjpeg
- libpng
- zlib
- [GifEncoder](https://github.com/xiaozhuai/GifEncoder)
  
The libraries are already provided in the `lib` directory. No further requirements or installation.

## Installation
The binaries for Linux and Windows environment are provided. To rebuild the program from source, the `make` tool is required. Execute the following command to rebuild
```
make all
```
The Makefile is tested on Windows and WSL2.

## How to run
The program can be executed by either of these commands
```
./bin/main
./bin/main.exe
```
Alternatively, the program can be executed by using `make`
```
make run
```
Enter your inputs per line as will be instructed. Specify input and output file path including the extension. Make sure that the directory of the output path exists.

##
Syahrizal Bani Khairan 13523063  
Tugas Kecil 2 IF2211 Strategi Algoritma

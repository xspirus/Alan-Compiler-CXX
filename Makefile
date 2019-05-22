MAKEFLAGS += --no-print-directory

.PHONY : build-release build-debug alan-release alan-debug clean distclean

default : alan-release

build-release :
	@echo "Executing CMake"
	@cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=RELEASE
	@echo "CMake done"

build-debug :
	@echo "Executing CMake"
	@cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=DEBUG
	@echo "CMake done"

lib :
	@cmake --build build --target alanstd

alan-release : build-release lib script
	@cmake --build build --target ALAN

alan-debug : build-debug lib script
	@cmake --build build --target ALAN

script:
	@make -C alancxx

clean :
	@rm -rf build
	@rm src/lexer/lexer.cpp src/parser/parser.hpp src/parser/parser.cpp src/parser/parser.output
	@echo "Removed build directory"

distclean : clean
	@rm -rf bin
	@rm -rf libs
	@echo "Removed bin and libs directory"

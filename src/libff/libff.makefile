


all:


clean:


distclean:
	@rm -fr Makefile CMakeCache.txt cmake_install.cmake CTestTestfile.cmake DartConfiguration.tcl CMakeFiles/ Testing/
	@rm -fr depends/Makefile depends/cmake_install.cmake depends/CTestTestfile.cmake depends/CMakeFiles/ depends/libzm.a
	@rm -fr libff/Makefile libff/cmake_install.cmake libff/CTestTestfile.cmake libff/CMakeFiles/ libff/libff.a

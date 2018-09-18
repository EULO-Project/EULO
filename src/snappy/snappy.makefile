
all:

clean:

distclean:
	@rm -fr CMakeCache.txt CTestTestfile.cmake SnappyConfigVersion.cmake cmake_install.cmake
	@rm -fr CMakeFiles/ Makefile snappy_unittest
	@rm -fr config.h snappy-stubs-public.h 
	@rm -fr libsnappy.a



all:

clean:

distclean:
	@rm -fr Makefile CMakeCache.txt cmake_install.cmake CPackConfig.cmake CPackSourceConfig.cmake libcontract.a CMakeFiles/
	@rm -fr libdevcore/Makefile libdevcore/cmake_install.cmake libdevcore/libdevcore.a libdevcore/CMakeFiles/
	@rm -fr libdevcore/Makefile libdevcore/cmake_install.cmake libdevcore/libdevcore.a libdevcore/CMakeFiles/
	@rm -fr libdevcrypto/Makefile libdevcrypto/cmake_install.cmake libdevcrypto/libdevcrypto.a libdevcrypto/CMakeFiles/
	@rm -fr libethashseal/Makefile libethashseal/cmake_install.cmake 
	@rm -fr libethcore/Makefile libethcore/cmake_install.cmake libethcore/libethcore.a libethcore/CMakeFiles/
	@rm -fr libethereum/Makefile libethereum/cmake_install.cmake  libethereum/CMakeFiles/
	@rm -fr libevm/Makefile libevm/cmake_install.cmake libevm/libevm.a libevm/CMakeFiles/


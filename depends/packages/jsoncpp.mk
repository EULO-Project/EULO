package=jsoncpp
$(package)_version=0.10.7
$(package)_download_path=https://github.com/open-source-parsers/$(package)/archive
$(package)_file_name=$($(package)_version).tar.gz
$(package)_sha256_hash=73e235c230708a8ac78ec11b886434a018f89691bd9e7fcf9c3128c8e677b435
$(package)_patches=$(package)-path.patch

define $(package)_set_vars
endef

define $(package)_config_cmds
  mkdir -p $($(package)_staging_prefix_dir) && \
  patch -p1 < $($(package)_patch_dir)/$(package)-path.patch && \
  cmake -DCMAKE_INSTALL_PREFIX=$($(package)_staging_prefix_dir) -DCMAKE_BUILD_TYPE=Release -DJSONCPP_WITH_TESTS=0 -DJSONCPP_WITH_POST_BUILD_UNITTEST=0 .
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  cmake -P cmake_install.cmake
endef

define $(package)_postprocess_cmds
endef

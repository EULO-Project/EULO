package=cryptopp
$(package)_version=5_6_5
$(package)_download_path=https://github.com/weidai11/$(package)/archive
$(package)_file_name=CRYPTOPP_$($(package)_version).tar.gz
$(package)_sha256_hash=79fd5514b3b191a1c6d934cd989d5e058f4726a72a3dad2444bd1274a6aae686

define $(package)_set_vars
endef

define $(package)_config_cmds
  mkdir -p $($(package)_staging_prefix_dir) && \
  cmake -DCMAKE_INSTALL_PREFIX=$($(package)_staging_prefix_dir) -DBUILD_STATIC=1 -DBUILD_SHARED=0 -DBUILD_TESTING=0 .
endef

define $(package)_build_cmds
  $(MAKE) -f Makefile
endef

define $(package)_stage_cmds
  $(MAKE) -f Makefile install && \
  rm -fr $($(package)_staging_prefix_dir)/lib/cmake/
endef

define $(package)_postprocess_cmds
endef

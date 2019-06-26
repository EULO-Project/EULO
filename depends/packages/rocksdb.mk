package=rocksdb
$(package)_version=5.18.3
$(package)_download_path=https://github.com/facebook/$(package)/archive
$(package)_file_name=v$($(package)_version).tar.gz
$(package)_sha256_hash=7fb6738263d3f2b360d7468cf2ebe333f3109f3ba1ff80115abd145d75287254

define $(package)_set_vars
endef

define $(package)_config_cmds
  mkdir -p $($(package)_staging_prefix_dir) && \
  cmake -DCMAKE_INSTALL_PREFIX=$($(package)_staging_prefix_dir) -DWITH_TOOLS=0 -DWITH_TESTS=0 .
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) install && \
  rm -fr $($(package)_staging_prefix_dir)/lib/cmake/ $($(package)_staging_prefix_dir)/lib/lib$(package).so*
endef

define $(package)_postprocess_cmds
endef

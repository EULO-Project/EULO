package=ethash
$(package)_version=0.4.1
$(package)_download_path=https://github.com/chfast/$(package)/archive
$(package)_file_name=v$($(package)_version).tar.gz
$(package)_sha256_hash=db70eb8549bbb57096f6e0e3ffcf7276683eb2c2eed66766149011deadaaa150

define $(package)_set_vars
endef

define $(package)_config_cmds
  cmake -DCMAKE_INSTALL_PREFIX=$($(package)_staging_prefix_dir) -DETHASH_BUILD_TESTS=0 . && \
  mkdir -p $($(package)_staging_prefix_dir)
endef

define $(package)_build_cmds
  $(MAKE) -C lib/$(package)/ 
endef

define $(package)_stage_cmds
  $(MAKE) -C lib/$(package)/ install && \
  rm -fr $($(package)_staging_prefix_dir)/lib/cmake
endef

define $(package)_postprocess_cmds
endef

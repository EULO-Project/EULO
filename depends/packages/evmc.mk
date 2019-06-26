package=evmc
$(package)_version=6.0.0
$(package)_download_path=https://github.com/ethereum/$(package)/archive
$(package)_file_name=v$($(package)_version).tar.gz
$(package)_sha256_hash=0dd98a0a05c58398599e7ddbba4a5f1259aba50e8ba76a8a4e3423d45f954a74
$(package)_patches=$(package)-instructions.patch

define $(package)_set_vars
endef

define $(package)_config_cmds
  mkdir -p $($(package)_staging_prefix_dir) && \
  patch -p1 < $($(package)_patch_dir)/$(package)-instructions.patch && \
  cmake -DCMAKE_INSTALL_PREFIX=$($(package)_staging_prefix_dir) -DEVMC_TESTING=0 -DEVMC_EXAMPLES=0  -DHUNTER_ENABLED=0 .
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) install && \
  rm -fr $($(package)_staging_prefix_dir)/lib/cmake/
endef

define $(package)_postprocess_cmds
endef

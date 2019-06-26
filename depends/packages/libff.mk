package=libff
$(package)_version=f2067162520f91438b44e71a2cab2362f1c3cab4
$(package)_download_path=https://github.com/scipr-lab/$(package)/archive
$(package)_file_name=$($(package)_version).tar.gz
$(package)_sha256_hash=1ff3f8f3b9b4e8ce9e58364aeb15ded88c3514562286a162478fbb9b2553243d
$(package)_dependencies=openssl gmp

define $(package)_set_vars
endef

define $(package)_config_cmds
  mkdir -p $($(package)_staging_prefix_dir) && \
  cmake -DCMAKE_INSTALL_PREFIX=$($(package)_staging_prefix_dir) -DOPENSSL_ROOT_DIR=$(host_prefix) -DGMP_INCLUDE_DIR=$(host_prefix)/include -DGMP_LIBRARY=$(host_prefix)/lib -DCURVE=ALT_BN128 -DWITH_PROCPS=0 -DUSE_ASM=0 -DIS_LIBFF_PARENT=0 .
endef

define $(package)_build_cmds
  $(MAKE) 
endef

define $(package)_stage_cmds
  $(MAKE) install && \
  rm -fr $($(package)_staging_prefix_dir)/include/$(package)/CMakeFiles
endef

define $(package)_postprocess_cmds
endef

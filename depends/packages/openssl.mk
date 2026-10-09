package=openssl
$(package)_version=3.5.9
$(package)_download_path=https://www.openssl.org/source
$(package)_file_name=$(package)-$($(package)_version).tar.gz
$(package)_sha256_hash=603f5602e2eef00d77fbd429d34dcd5822bb301757a1bc9cdb24c670f1eb859a
$(package)_patches=build-info-prefix.patch

define $(package)_set_vars
$(package)_config_env=CC="$($(package)_cc)" CXX="$($(package)_cxx)" AR="$($(package)_ar)" RANLIB="$($(package)_ranlib)" SOURCE_DATE_EPOCH=1
$(package)_build_env=SOURCE_DATE_EPOCH=1 OPENSSL_BUILD_PREFIX=$(host_prefix)
$(package)_config_opts=--prefix=$(host_prefix) --libdir=lib --openssldir=/etc/ssl
# Providers are built into libcrypto; no runtime modules/engines or user config.
$(package)_config_opts+=no-shared no-module no-dso no-engine no-autoload-config
# Preserve applicable exclusions from the old recipe. Removed options no longer
# exist in the unified build; default TLS security checks remain enabled.
$(package)_config_opts+=no-camellia no-cast no-comp no-dtls1 no-idea no-md2 no-mdc2
$(package)_config_opts+=no-rc4 no-rc5 no-rfc3779 no-sctp no-seed no-ssl3
$(package)_config_opts+=no-ssl-trace no-weak-ssl-ciphers no-whirlpool no-zlib
$(package)_config_opts+=$($(package)_cflags) $($(package)_cppflags)
$(package)_config_opts_linux=-fPIC -Wa,--noexecstack
$(package)_config_opts_x86_64_linux=linux-x86_64
$(package)_config_opts_i686_linux=linux-generic32
$(package)_config_opts_arm_linux=linux-generic32
$(package)_config_opts_aarch64_linux=linux-generic64
$(package)_config_opts_mipsel_linux=linux-generic32
$(package)_config_opts_mips_linux=linux-generic32
$(package)_config_opts_powerpc_linux=linux-generic32
$(package)_config_opts_x86_64_darwin=darwin64-x86_64-cc
$(package)_config_opts_x86_64_mingw32=mingw64
$(package)_config_opts_i686_mingw32=mingw
endef

define $(package)_preprocess_cmds
  patch -p1 -i $($(package)_patch_dir)/build-info-prefix.patch
endef

define $(package)_config_cmds
  ./Configure $($(package)_config_opts)
endef

define $(package)_build_cmds
  $(MAKE) ENGINESDIR=/lib/engines-3 MODULESDIR=/lib/ossl-modules build_libs
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install_dev
endef

define $(package)_postprocess_cmds
  rm -rf share bin lib/cmake
endef

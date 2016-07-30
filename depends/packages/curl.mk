package=curl
$(package)_version=7.50.0
$(package)_download_path=https://curl.haxx.se/download
$(package)_file_name=curl-$(curl_version).tar.bz2
$(package)_sha256_hash=608dfe2db77f48db792c387e7791aca55a25f0b42385707ad927164199ecfa9a

#define $(package)_preprocess_cmds
#  ./autogen.sh
#endef

define $(package)_set_vars
  $(package)_config_opts=--disable-shared
  $(package)_config_opts_linux=--with-pic
endef

define $(package)_config_cmds
  $($(package)_autoconf)
endef

define $(package)_build_cmds
  $(MAKE) V=1
endef

#define $(package)_stage_cmds
#  $(MAKE) install
#endef

#define $(package)_stage_cmds
#  $(MAKE) DESTDIR=$($(package)_staging_dir) install
#endef

define $(package)_stage_cmds
  mkdir -p $($(package)_staging_prefix_dir)/include/curl $($(package)_staging_prefix_dir)/lib &&\
  install include/curl/*.h $($(package)_staging_prefix_dir)/include/curl &&\
  install lib/.libs/libcurl.a $($(package)_staging_prefix_dir)/lib &&\
  $(MAKE) DESTDIR=$($(package)_staging_dir) install
endef


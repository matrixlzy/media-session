systemd_system_services_dir = systemd.get_pkgconfig_variable('systemdsystemunitdir', define_variable : [ 'prefix', prefix])

systemd_config = configuration_data()
systemd_config.set('PW_MEDIA_SESSION_BINARY', media_session_bindir / 'pipewire-media-session')

if get_option('session-managers').contains('media-session')
  configure_file(input : 'pipewire-media-session.service.in',
               output : 'pipewire-media-session.service',
               configuration : systemd_config,
               install_dir : systemd_system_services_dir)
endif

systemd_user_services_dir = systemd.get_pkgconfig_variable('systemduserunitdir', define_variable : [ 'prefix', prefix])
if get_option('systemd-user-unit-dir') != ''
  systemd_user_services_dir = get_option('systemd-user-unit-dir')
endif

systemd_config = configuration_data()
systemd_config.set('PW_MEDIA_SESSION_BINARY', media_session_bindir / 'pipewire-media-session')

configure_file(input : 'pipewire-media-session.service.in',
              output : 'pipewire-media-session.service',
              configuration : systemd_config,
              install_dir : systemd_user_services_dir)

systemd_system_services_dir = systemd.get_variable(pkgconfig: 'systemdsystemunitdir',
                                                   pkgconfig_define: [ 'prefix', prefix])

systemd_config = configuration_data()
systemd_config.set('PW_MEDIA_SESSION_BINARY', media_session_bindir / 'pipewire-media-session')

configure_file(input : 'pipewire-media-session.service.in',
             output : 'pipewire-media-session.service',
             configuration : systemd_config,
             install_dir : systemd_system_services_dir)

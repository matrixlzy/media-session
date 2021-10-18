project('media-session', ['c'],
  version : '0.3.38',
  license : [ 'MIT', 'LGPL-2.1-or-later', 'GPL-2.0-only' ],
  meson_version : '>= 0.54.0',
  default_options : [ 'warning_level=3',
                      'c_std=gnu99',
                      'b_pie=true',
                      #'b_sanitize=address,undefined',
                      'buildtype=debugoptimized' ])

media_session_version = meson.project_version()
version_arr = media_session_version.split('.')
media_session_version_major = version_arr[0]
media_session_version_minor = version_arr[1]
media_session_version_micro = version_arr[2]
if version_arr.length() == 4
  media_session_version_nano = version_arr[3]
else
  media_session_version_nano = 0
endif

pipewire_name = 'pipewire'

prefix = get_option('prefix')
media_session_bindir = prefix / get_option('bindir')
media_session_datadir = prefix / get_option('datadir')
media_session_libdir = prefix / get_option('libdir')
media_session_libexecdir = prefix / get_option('libexecdir')
media_session_localedir = prefix / get_option('localedir')
media_session_sysconfdir = prefix / get_option('sysconfdir')

media_session_configdir = media_session_sysconfdir / 'pipewire'
media_session_confdatadir = media_session_datadir / 'pipewire'

gnome = import('gnome')
pkgconfig = import('pkgconfig')

cc = meson.get_compiler('c')

common_flags = [
  '-fvisibility=hidden',
  '-Werror=suggest-attribute=format',
  '-Wsign-compare',
  '-Wpointer-arith',
  '-Wpointer-sign',
  '-Wformat',
  '-Wformat-security',
  '-Wimplicit-fallthrough',
  '-Wmissing-braces',
  '-Wtype-limits',
  '-Wvariadic-macros',
  '-Wno-missing-field-initializers',
  '-Wno-unused-parameter',
  '-Wno-pedantic',
  '-Wold-style-declaration',
  '-Wunused-result',
]

cc_flags = common_flags + [
  '-D_GNU_SOURCE',
]
add_project_arguments(cc.get_supported_arguments(cc_flags), language: 'c')

have_cpp = add_languages('cpp', native: false, required : false)

if have_cpp
  cxx = meson.get_compiler('cpp')
  cxx_flags = common_flags
  add_project_arguments(cxx.get_supported_arguments(cxx_flags), language: 'cpp')
endif

cdata = configuration_data()
cdata.set_quoted('MEDIA_SESSION_CONFDATADIR', media_session_confdatadir)
cdata.set_quoted('LOCALEDIR', media_session_localedir)
cdata.set_quoted('LIBDIR', media_session_libdir)
cdata.set_quoted('GETTEXT_PACKAGE', meson.project_name())
cdata.set_quoted('PACKAGE', 'media-session')
cdata.set_quoted('PACKAGE_NAME', '"media-session"')
cdata.set_quoted('PACKAGE_STRING', 'media-session @0@'.format(media_session_version))
cdata.set_quoted('PACKAGE_TARNAME', 'media-session')
cdata.set_quoted('PACKAGE_URL', '"https://pipewire.org"')
cdata.set_quoted('PACKAGE_VERSION', media_session_version)
cdata.set_quoted('PIPEWIRE_CONFIG_DIR', media_session_configdir)
# FIXME: --with-memory-alignment],[8,N,malloc,pagesize (default is 32)]) option
cdata.set('MEMORY_ALIGNMENT_MALLOC', 1)

if host_machine.endian() == 'big'
  cdata.set('WORDS_BIGENDIAN', 1)
endif

check_headers = [['dlfcn.h','HAVE_DLFCN_H'],
  ['inttypes.h', 'HAVE_INTTYPES_H'],
  ['memory.h', 'HAVE_MEMORY_H'],
  ['poll.h', 'HAVE_POLL_H'],
  ['stddef.h', 'HAVE_STDDEF_H'],
  ['stdint.h', 'HAVE_STDINT_H'],
  ['stdio_ext.h', 'HAVE_STDIO_EXT_H'],
  ['strings.h', 'HAVE_STRINGS_H'],
  ['string.h', 'HAVE_STRING_H'],
  ['sys/mount.h', 'HAVE_SYS_MOUNT_H'],
  ['sys/param.h', 'HAVE_SYS_PARAM_H'],
  ['sys/poll.h', 'HAVE_SYS_POLL_H'],
  ['sys/prctl.h', 'HAVE_SYS_PRCTL_H'],
  ['sys/random.h', 'HAVE_SYS_RANDOM_H'],
  ['sys/socket.h', 'HAVE_SYS_SOCKET_H'],
  ['sys/stat.h', 'HAVE_SYS_STAT_H'],
  ['sys/times.h', 'HAVE_SYS_TIMES_H'],
  ['sys/time.h', 'HAVE_SYS_TIME_H'],
  ['sys/types.h', 'HAVE_SYS_TYPES_H'],
  ['sys/utsname.h', 'HAVE_SYS_UTSNAME_H'],
  ['sys/vfs.h', 'HAVE_SYS_VFS_H'],
  ['sys/wait.h', 'HAVE_SYS_WAIT_H'],
  ['pwd.h', 'HAVE_PWD_H'],
  ['ucontext.h', 'HAVE_UCONTEXT_H'],
  ['unistd.h', 'HAVE_UNISTD_H'],
]

foreach h : check_headers
  if cc.has_header(h.get(0))
    cdata.set(h.get(1), 1)
  endif
endforeach

if cc.has_function('poll', prefix : '#include<poll.h>')
  cdata.set('HAVE_POLL', 1)
endif
if cc.has_function('pselect', prefix : '#include<sys/select.h>')
  cdata.set('HAVE_PSELECT', 1)
endif
cdata.set('HAVE_MMAP', 1)

if cc.has_function('posix_memalign', prefix : '#include<stdlib.h>')
  cdata.set('HAVE_POSIX_MEMALIGN', 1)
endif
if cc.has_function('getpagesize', prefix : '#include<unistd.h>')
  cdata.set('HAVE_GETPAGESIZE', 1)
endif
if cc.has_function('gettid', prefix : '#include<unistd.h>', args: [ '-D_GNU_SOURCE' ])
  cdata.set('HAVE_GETTID', 1)
endif
if cc.has_function('clock_gettime', prefix : '#include <time.h>')
  cdata.set('HAVE_CLOCK_GETTIME', 1)
endif

if cc.has_type('ptrdiff_t', prefix : '#include <stddef.h>')
  cdata.set('HAVE_PTRDIFF_T', 1)
endif

if cc.has_header_symbol('string.h', 'strndupa', args : [ '-D_GNU_SOURCE' ])
  cdata.set('HAVE_STRNDUPA', 1)
endif

if cc.has_function('mkstemp', prefix : '#include <stdlib.h>')
  cdata.set('HAVE_MKSTEMP', 1)
endif

if cc.has_function('memfd_create', prefix : '#include <sys/mman.h>', args : [ '-D_GNU_SOURCE' ])
  cdata.set('HAVE_MEMFD_CREATE', 1)
endif

if cc.has_function('getrandom', prefix : '#include <stddef.h>\n#include <sys/random.h>', args : [ '-D_GNU_SOURCE' ])
  cdata.set('HAVE_GETRANDOM', 1)
endif

if cc.has_function('sigabbrev_np', prefix : '#include <string.h>', args : [ '-D_GNU_SOURCE' ])
  cdata.set('HAVE_SIGABBREV_NP', 1)
endif

if cc.get_define('SYS_pidfd_open', prefix : '#include <sys/syscall.h>') != ''
  cdata.set('HAVE_PIDFD_OPEN', 1)
endif

pipewire_dep = dependency('libpipewire-0.3')

systemd = dependency('systemd', required: get_option('systemd'))
systemd_dep = dependency('libsystemd',required: get_option('systemd'))
summary({'systemd conf data': systemd.found()}, bool_yn: true)
summary({'libsystemd': systemd_dep.found()}, bool_yn: true)
if systemd.found() and systemd_dep.found()
  cdata.set('HAVE_SYSTEMD', 1)
endif

configinc = include_directories('.')
pipewire_inc = include_directories('src')

makedata = configuration_data()
  makedata.set('BUILD_ROOT', meson.build_root())
  makedata.set('SOURCE_ROOT', meson.source_root())
  makedata.set('VERSION', media_session_version)
if version_arr.length() == 4
  makedata.set('TAG', 'HEAD')
else
  makedata.set('TAG', media_session_version)
endif

# Find dependencies
mathlib = cc.find_library('m', required : false)
rt_lib = cc.find_library('rt', required : false) # clock_gettime
dl_lib = cc.find_library('dl', required : false)
pthread_lib = dependency('threads')
dbus_dep = dependency('dbus-1')

cap_lib = dependency('libcap', required : false)
if cap_lib.found()
  cdata.set('HAVE_LIBCAP', 1)
endif

# On FreeBSD, epoll-shim library is required for eventfd() and timerfd()
epoll_shim_dep = (build_machine.system() == 'freebsd'
    ? dependency('epoll-shim', required: true)
    : dependency('', required: false))

libinotify_dep = (build_machine.system() == 'freebsd'
    ? dependency('libinotify', required: true)
    : dependency('', required: false))

# On FreeBSD, libintl library is required for gettext
libintl_dep = dependency('intl', required: false)

if not libintl_dep.found()
    libintl_dep = cc.find_library('intl', required: false)
endif
summary({'intl support': libintl_dep.found()}, bool_yn: true)

alsa_dep = dependency('alsa', version : '>=1.1.7')

subdir('po')
subdir('src')
subdir('media-session.d')
if systemd.found()
  subdir('systemd')
endif

configure_file(output : 'config.h',
               configuration : cdata)

doxygen = find_program('doxygen', required : get_option('docs'))
if doxygen.found()
  subdir('doc')
endif

setenv = find_program('media-session-uninstalled.sh')
run_target('media-session-uninstalled',
           command : [setenv,
                      '-b@0@'.format(meson.build_root()),
                      '-v@0@'.format(media_session_version)]
)

if meson.version().version_compare('>=0.58.0')
  devenv = environment()

  builddir = meson.current_build_dir()
  srcdir = meson.current_source_dir()

  devenv.set('MEDIA_SESSION_CONFIG_DIR', builddir / 'src' / 'media-session' / 'media-session.d')

  devenv.set('PW_UNINSTALLED', '1')

  meson.add_devenv(devenv)
endif

## Building

PipeWire Media Session uses a build tool called *Meson* as a basis for its build
process.  It's a tool with some resemblance to Autotools and CMake. Meson
again generates build files for a lower level build tool called *Ninja*,
working in about the same level of abstraction as more familiar GNU Make
does.

Meson uses a user-specified build directory and all files produced by Meson
are in that build directory. This build directory will be called `builddir`
in this document.

Generate the build files for Ninja:

```
$ meson setup builddir
```

For distribution-specific build dependencies, please check our
[CI pipeline](https://gitlab.freedesktop.org/pipewire/media-session/-/blob/master/.gitlab-ci.yml)
(search for `FDO_DISTRIBUTION_PACKAGES`). Note that some dependencies are
optional and depend on options passed to meson.

Once this is done, the next step is to review the build options:

```
$ meson configure builddir
```

Define the installation prefix:

```
$ meson configure builddir -Dprefix=/usr # Default: /usr/local
```

PipeWire Media Session specific build options are listed in the "Project options"
section. They are defined in `meson_options.txt`.

Finally, invoke the build:

```
$ ninja -C builddir
```

Just to avoid any confusion: `autogen.sh` is a script invoked by *Jhbuild*,
which orchestrates multi-component builds.

## Running

If you want to run PipeWire Media Session without installing it on your system, there is a
script that you can run. This puts you in an environment in which PipeWire Media Session can
be run from the build directory. You can get into this environment with:

```
$ ./media-session-uninstalled.sh
$ pipewire-media-session
```

This will use the default config file to configure and start PipeWire Media
Session.

You can also enable more debugging with the PIPEWIRE_DEBUG environment
variable like so:

```
cd builddir/
PIPEWIRE_DEBUG="D" make run
```

You might have to stop the pipewire-media-session service that might have been
started already, with:

```
systemctl --user stop pipewire-media-session.service
```

## Installing

Inside `builddir`, run:

```
sudo meson install
```

to install everything onto the system into the specified prefix.
Some additional steps will have to be performed to integrate
with the distribution as shown below.

# packbunch - a simple way to manage your apt packages
Packbunch is a program that helps you install and remove multiple apt packages as a group, making it easier to remember what packages were used for a certain task or project and allowing for quicker cleanup after that's finished. I initially created it for personal use, but then thought someone else might also find it useful, so it ended up here.
### IMPORTANT: packbunch is only intended to be used with the apt package manager and will not work on systems that use a different package manager

## Basic usage
First off, you'll need to tell packbunch what packages you want to manage as a group. You can do so by creating a "bunch" using the following command:

`packbunch create name`
- name - The name you want to give to the bunch

Then adding packages to it like so:

`packbunch add bunch packages`
- bunch - Name of the bunch you want to add the packages to
- packages - One or more package names, separate by spaces

At this point, you can install the bunch (which means installing all the packages in it) with this command:

`sudo packbunch install bunch`
- bunch - Name of the bunch you want to install

This will try to install the packages you added using apt.

If you want to uninstall those packages, you can simply do:

`sudo packbunch uninstall bunch`
- bunch - Name of the bunch you want to uninstall

If you want to remove packages from the bunch (not uninstalling them, just telling packbunch to stop managing them), use this command:

`packbunch remove bunch packages`
- bunch - Name of the bunch you want to remove packages from
- packages - One or more package names, separated by spaces

If you want to completely get rid of a bunch, use this command:

`sudo packbunch delete bunch`
- bunch - Name of the bunch you want to delete

The `delete` command will also ask if you want to uninstall the bunch first (this is recommended). You can answer with "y" (yes) or "n" (no).

If you want to see a list of all bunches, use this command:

`packbunch list`

If you want to see what packages are in a bunch, use this command:

`packbunch view bunch`
- bunch - Name of the bunch you want to list the packages from

## External bunches
If you want to use a bunch that someone else made, or that you have stored in an external location, you can import it with this command:

`packbunch import path`
- path - Location of the bunch you want to import

Note that the imported bunch will have the same name as the file at the external location.

If you want to give a bunch to someone else or store it in an external location, you can export it with this command:

`packbunch export bunch path`
- bunch - Name of the bunch you want to export
- path - Location where you want the bunch to be exported (must be a directory, the new file will be created with the same name as the bunch)

## Other commands
`packbunch help` - Shows a basic help menu

`packbunch version`- Shows the packbunch version you have installed

## How to install
### Prerequisites
To install packbunch, you'll need to install the build-essential package first (you can later remove it):

`sudo apt install build-essential`

If you choose to install packbunch using the install script, you'll also need curl:

`sudo apt install curl`

### Install script
Probably the easiest way to install packbunch, just run the following command in your terminal and you should be good to go:

`curl https://raw.githubusercontent.com/tudorconst/packbunch/refs/heads/main/install.sh | sh`

Alternatively, you can download `install.sh` and run that.

#### Uninstallation
If you want to uninstall packbunch, run this command:

`curl https://raw.githubusercontent.com/tudorconst/packbunch/refs/heads/main/uninstall.sh | sh`

Or download `uninstall.sh` and run it.

### Manual install
1. Download `packbunch.cpp` and `Makefile` and place them in the directory where you want to install packbunch.
2. Open a terminal there and  run the `make` command.
3. Add `export PATH=$PATH:path` to the `.bashrc` file in your home directory, where `path` is the path to your installation directory. This step is optional, but recommended, as it's what allows you to use packbunch from anywhere on the system.

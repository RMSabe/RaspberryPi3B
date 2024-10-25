To build these codes, you'll need the kernel header files.

On Raspbian/Raspberry Pi OS, the package containing the header files is "raspberrypi-kernel-headers"

On other distros, the kernel headers are usually in package linux-headers-<kernel version>. "linux-headers-$(uname -r)"

Once the code is compiled, it should generate a kernel object file ".ko".

To enable the driver, run "sudo insmod driver.ko".
To disable it, run "sudo rmmod driver.ko".
To list running drivers, use "lsmod".


# Kernel module debugging

Create a simple string uppercase converter using procfs
(write: input string, read: that string in uppercase).

Create a simple string lowercase converter using sysfs
(store: input string, show: that string in lowercase).

Add read only attributes for both filesystems
showing statistics (using cat):
e.g. total calls, total characters processed, characters converted etc.

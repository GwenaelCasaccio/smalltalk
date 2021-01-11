Mirror_FILES = \
packages/mirror/Isolation.st packages/mirror/kernel/kernel.st 
$(Mirror_FILES):
$(srcdir)/packages/mirror/stamp-classes: $(Mirror_FILES)
	touch $(srcdir)/packages/mirror/stamp-classes

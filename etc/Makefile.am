install-data-local:
	if [ ! -d $(prefix)/bin/etc ]; then $(INSTALL) -d $(prefix)/bin/etc; fi
	$(INSTALL_DATA) system.db $(prefix)/bin/etc/system.db

EXTRA_DIST = cellphone.xml dial.skin gvfb.skin system.db

install-data-local:
	if [ ! -d $(prefix)/bin/etc ]; then $(INSTALL) -d $(prefix)/bin/etc; fi
	$(INSTALL_DATA) system.db $(prefix)/bin/etc/system.db
	$(INSTALL_DATA) cellphone.xml $(prefix)/bin/etc/cellphone.xml
	$(INSTALL_DATA) dial.skin $(prefix)/bin/etc/dial.skin
	$(INSTALL_DATA) gvfb.skin $(prefix)/bin/etc/gvfb.skin

EXTRA_DIST = cellphone.xml dial.skin gvfb.skin system.db

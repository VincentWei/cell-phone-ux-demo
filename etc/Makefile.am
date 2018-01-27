install-data-local:
	if [ ! -d $(prefix)/bin/etc ]; then $(INSTALL) -d $(prefix)/bin/etc; fi
	$(INSTALL_DATA) system.db $(prefix)/bin/etc/system.db
	$(INSTALL_DATA) cellphone.xml $(prefix)/bin/etc/cellphone.xml
	$(INSTALL_DATA) dial.xml $(prefix)/bin/etc/dial.xml
	$(INSTALL_DATA) gvfb.xml $(prefix)/bin/etc/gvfb.xml

EXTRA_DIST = cellphone.xml dial.skin gvfb.skin system.db

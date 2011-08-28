SUBDIRS  = src 

RPMSPECS = foobillardplus.spec  

EXTRA_DIST = $(RPMSPECS)

rpm: dist
	mkdir -p build
	PWD=`pwd` rpmbuild --define "_sourcedir $$PWD" -bb foobillardplus.spec 
					    
srpm: dist
	mkdir -p build
	PWD=`pwd` rpmbuild --define "_sourcedir $$PWD" --define "_builddir $$PWD/build" \
	  --define "_rpmdir $$PWD/build" --define "_srcrpmdir $$PWD" -bs foobillardplus.spec 

dist-hook:
	mkdir -p $(distdir)/data
	cp -p $(srcdir)/data/* $(distdir)/data -R
	cp -p $(srcdir)/AUTHORS $(distdir)
	cp -p $(srcdir)/COPYING $(distdir)
	cp -p $(srcdir)/INSTALL $(distdir)
	cp -p $(srcdir)/README $(distdir)
	cp -p $(srcdir)/ChangeLog $(distdir)
	cp -p $(srcdir)/TODO $(distdir)
	cp -p $(srcdir)/foobillardplus.desktop $(distdir)
	cp -p $(srcdir)/foobillardplus.png $(distdir)
	cp -p $(srcdir)/foobillardplus.xbm $(distdir)

install-data-hook:
	mkdir -p $(datadir)
	cp -p $(srcdir)/data/* $(datadir) -R
	cp -p $(srcdir)/AUTHORS $(prefix)
	cp -p $(srcdir)/COPYING $(prefix)
	cp -p $(srcdir)/INSTALL $(prefix)
	cp -p $(srcdir)/README $(prefix)
	cp -p $(srcdir)/ChangeLog $(prefix)
	cp -p $(srcdir)/TODO $(prefix)
	cp -p $(srcdir)/foobillardplus.desktop $(distdir)
	cp -p $(srcdir)/foobillardplus.png $(distdir)
	cp -p $(srcdir)/foobillardplus.xbm $(distdir)

uninstall-hook:
	rm $(prefix) -R

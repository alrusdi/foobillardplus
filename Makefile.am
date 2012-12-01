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
	cp -p $(srcdir)/OFL.txt $(distdir)
	cp -p $(srcdir)/foobillardplus.desktop $(distdir)
	cp -p $(srcdir)/foobillardplus.desktop /usr/share/applications
	cp -p $(srcdir)/foobillardplus.png $(distdir)
	cp -p $(srcdir)/foobillardplus.png /usr/share/icons
	cp -p $(srcdir)/foobillardplus.xbm $(distdir)

install-data-hook:
	mkdir -p $(deb_share_dir)$(prefix)/data
	cp -p $(srcdir)/data/* $(deb_share_dir)$(prefix)/data -R
	cp -p $(srcdir)/AUTHORS $(deb_share_dir)$(prefix)
	cp -p $(srcdir)/COPYING $(deb_share_dir)$(prefix)
	cp -p $(srcdir)/INSTALL $(deb_share_dir)$(prefix)
	cp -p $(srcdir)/README $(deb_share_dir)$(prefix)
	cp -p $(srcdir)/ChangeLog $(deb_share_dir)$(prefix)
	cp -p $(srcdir)/TODO $(deb_share_dir)$(prefix)
	cp -p $(srcdir)/OFL.txt $(deb_share_dir)$(prefix)
	cp -p $(srcdir)/foobillardplus.desktop $(deb_share_dir)$(prefix)
	mkdir -p $(deb_root_dir)/usr/share/applications
	cp -p $(srcdir)/foobillardplus.desktop $(deb_root_dir)/usr/share/applications
	cp -p $(srcdir)/foobillardplus.png $(deb_share_dir)$(prefix)
	mkdir -p $(deb_root_dir)/usr/share/icons
	cp -p $(srcdir)/foobillardplus.png $(deb_root_dir)/usr/share/icons
	cp -p $(srcdir)/foobillardplus.xbm $(deb_share_dir)$(prefix)

uninstall-hook:
	rm $(prefix) -R


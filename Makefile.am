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
	mkdir -p $(datadir)
	cp -p $(srcdir)/data/* $(datadir) -R
	cp -p $(srcdir)/AUTHORS $(datarootdir)
	cp -p $(srcdir)/COPYING $(datarootdir)
	cp -p $(srcdir)/INSTALL $(datarootdir)
	cp -p $(srcdir)/README $(datarootdir)
	cp -p $(srcdir)/ChangeLog $(datarootdir)
	cp -p $(srcdir)/TODO $(datarootdir)
	cp -p $(srcdir)/OFL.txt $(datarootdir)
	cp -p $(srcdir)/foobillardplus.desktop $(datarootdir)
	cp -p $(srcdir)/foobillardplus.png $(datarootdir)
	cp -p $(srcdir)/foobillardplus.png /usr/share/icons
	cp -p $(srcdir)/foobillardplus.xbm $(datarootdir)
	echo -e "[Desktop Entry]\nVersion=1.0\nType=Application\nTerminal=false\nIcon=/usr/share/icons/foobillardplus.png\nName=Foobillard++\nExec=$(bindir)/foobillardplus\nComment=OpenGL Billiard Game\nGenericName=OpenGL Billiard Game\n" >/usr/share/applications/foobillardplus.desktop

uninstall-hook:
	rm $(datarootdir) -R
	rm /usr/share/icons/foobillardplus.png
	rm /usr/share/applications/foobillardplus.desktop

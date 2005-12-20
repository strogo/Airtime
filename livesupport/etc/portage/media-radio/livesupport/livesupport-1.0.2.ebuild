# Copyright 1999-2005 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header$

inherit eutils flag-o-matic

IUSE=""

DESCRIPTION="LiveSupport is a radio broadcast support tool."
HOMEPAGE="http://livesupport.campware.org/"
SRC_URI="mirror://sourceforge/${PN}/${P}.tar.bz2"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~amd64 ~ppc ~sparc ~x86"

RESTRICT="maketest"

DEPEND=">=dev-db/unixODBC-2.2
	media-libs/fontconfig
	>=media-libs/libpng-1.2
	media-libs/jpeg
	dev-libs/openssl
	dev-libs/libxml2
	dev-libs/popt
	=dev-util/cppunit-1.10.2-r1
	>=dev-libs/icu-3.0
	media-libs/alsa-lib
	media-libs/libid3tag
	media-libs/libmad
	media-libs/libogg
	media-libs/libvorbis
	>=dev-libs/boost-0.31
	sys-apps/sed
	net-www/apache
	dev-lang/php
	>=dev-php/PEAR-PEAR-1.3.5
	>=dev-php/PEAR-Archive_Tar-1.3.1
	>=dev-php/PEAR-Calendar-0.5.2
	>=dev-php/PEAR-Console_Getopt-1.2
	>=dev-php/PEAR-DB-1.7.6
	>=dev-php/PEAR-File-1.2.0
	>=dev-php/PEAR-File_Find-1.2.0
	>=dev-php/PEAR-HTML_Common-1.2.1-r1
	>=dev-php/PEAR-HTML_QuickForm-3.2.5
	>=dev-php/PEAR-XML_Beautifier-1.1
	>=dev-php/PEAR-XML_Parser-1.2.7
	>=dev-php/PEAR-XML_RPC-1.4.0
	>=dev-php/PEAR-XML_Serializer-0.15
	>=dev-php/PEAR-XML_Util-1.1.1
	>=dev-db/postgresql-7.4
	>=x11-libs/gtk+-2.6.1
	>=dev-cpp/gtkmm-2.5.5
	>=net-misc/curl-7.13.2
	>=dev-cpp/libxmlpp-2.8.1
	=dev-db/libodbc++-0.2.3-r2
	=dev-libs/xmlrpc++-0.7
	=media-libs/gst-plugins-0.8.10-r1
	=media-libs/taglib-1.3.1-r3
	=media-plugins/gst-plugins-mad-0.8.10-r1
	=media-plugins/gst-plugins-ogg-0.8.10
	=media-libs/gstreamer-0.8.10"

src_unpack() {
	unpack ${A}
	cd ${S}

	# toch the tools make stamp, so that tools don't get built
	touch tmp/tools_setup.stamp
}

src_compile() {
	# append -g, otherwise we get 'defined in discared section' linkage errors
	append-flags -g

	econf --with-create-database=no \
		  --with-create-odbc-data-source=no \
		  --with-init-database=no \
		  --with-configure-apache=no \
		  --with-apache-group=apache \
		  --with-www-docroot=${D}/var/www/localhost/htdocs \
		|| die "configure failed"
	emake -j1 || die "make failed"
}

src_install() {
	# to make sure the link from the docroot works
	mkdir -p ${D}/var/www/localhost/htdocs
	emake -j1 prefix=${D}/usr install || die "install failed"
	dodoc doc INSTALL README
}

pkg_postinst() {
	/usr/bin/postInstallStation.sh --directory /usr \
	                               --www-root /var/www/localhost/htdocs \
	                               --apache-group apache \
	                               --postgresql-dir /var/lib/postgresql/data

	# we don't need the post-install script anymore...
	rm -f /usr/bin/postInstallStation.sh

	# an ugly hack: get rid of the temporary ${prefix} used during the
	# ebuild sandbox installation. this usually looks like
	# /var/tmp/portage/livesupport-<version>/image//usr
	# which will be replaced to /usr
	sedstr="s/\/var.*\/image\/\/usr/\/usr/"

	cat /usr/etc/scheduler.xml | sed -e $sedstr > /tmp/scheduler.xml.$$
	mv -f /tmp/scheduler.xml.$$ /usr/etc/scheduler.xml

	cat /usr/etc/gLiveSupport.xml | sed -e $sedstr > /tmp/gLiveSupport.xml.$$
	mv -f /tmp/gLiveSupport.xml.$$ /usr/etc/gLiveSupport.xml

	cat /usr/var/LiveSupport/storageAdmin/var/conf.php | sed -e $sedstr > /tmp/conf.php.$$
	mv -f /tmp/conf.php.$$ /usr/var/LiveSupport/storageAdmin/var/conf.php

	cat /usr/var/LiveSupport/archiveServer/var/conf.php | sed -e $sedstr > /tmp/conf.php.$$
	mv -f /tmp/conf.php.$$ /usr/var/LiveSupport/archiveServer/var/conf.php

	cat /usr/var/LiveSupport/storageServer/var/conf.php | sed -e $sedstr > /tmp/conf.php.$$
	mv -f /tmp/conf.php.$$ /usr/var/LiveSupport/storageServer/var/conf.php

	cat /usr/bin/import.sh | sed -e $sedstr > /tmp/import.sh.$$
	mv -f /tmp/import.sh.$$ /usr/bin/import.sh
	chmod +x /usr/bin/import.sh

	cat /usr/bin/dumpDbSchema.sh | sed -e $sedstr > /tmp/dumpDbSchema.sh.$$
	mv -f /tmp/dumpDbSchema.sh.$$ /usr/bin/dumpDbSchema.sh
	chmod +x /usr/bin/dumpDbSchema.sh

	cat /usr/bin/backup.sh | sed -e $sedstr > /tmp/backup.sh.$$
	mv -f /tmp/backup.sh.$$ /usr/bin/backup.sh
	chmod +x /usr/bin/backup.sh

	cat /usr/bin/restore.sh | sed -e $sedstr > /tmp/restore.sh.$$
	mv -f /tmp/restore.sh.$$ /usr/bin/restore.sh
	chmod +x /usr/bin/restore.sh
}


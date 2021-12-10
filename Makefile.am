## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.1 2004/08/18 16:32:53 flux Exp $

## stuff in shtest should get distributed but not installed
SUBDIRS = . tingea src shed

VERSION = @VERSION@

configure.ac: configure.ac.in VERSION
	shed/setversion

EXTRA_DIST = \
   LICENSE VERSION README COPYING THANKS INSTALL AUTHORS \
   configure.ac


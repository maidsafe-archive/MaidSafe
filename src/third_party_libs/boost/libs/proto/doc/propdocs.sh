#!/bin/sh

rsync --delete --rsh=ssh --recursive -p html/* eric_niebler,boost-sandbox@web.sourceforge.net:/home/groups/b/bo/boost-sandbox/htdocs/libs/proto/doc/html

rsync --delete --rsh=ssh --recursive -p src/* eric_niebler,boost-sandbox@web.sourceforge.net:/home/groups/b/bo/boost-sandbox/htdocs/libs/proto/doc/src

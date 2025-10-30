#!/bin/sh
# update version info inside kernel
cp include/version_master.h include/version.h
sed -i "s|<version>|0.03-1|g" include/version.h; sed -i "s|<placeholder>|$(date) $(whoami)@$(hostname):$(pwd)|g" include/version.h

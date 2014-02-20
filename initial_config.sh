#!/bin/bash

# setup the no overwrite of fresh_project.ewp

cd ~/ee472
echo 'removing setup files from git tracking......'

# This file maintains the set of files included in the project
git update-index --assume-unchanged fresh_project/fresh_project.ewp
echo 'setup file: fresh_project.ewp ...'

## this file is produced during debugs/uploading
#echo 'setup file: fresh_project.dni ...'
#git update-index --assume-unchanged fresh_project/settings/fresh_project.dni

## this file sets the debug options, shouldn't be changing though.
#echo 'fresh_projecct.ewd
#git update-index --assume-unchanged fresh_project/fresh_project.ewd

echo '...done'

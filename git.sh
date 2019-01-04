#!/bin/bash  
git pull
git add . 
git commit -m "$*"
git push https://github.com/guntisl/chicken_hood.git --force

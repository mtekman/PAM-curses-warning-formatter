#!/bin/bash

./buildInstallPamScript.sh
./buildWarner.sh


for f in `find ./configs -type f`; do
   new_loc=`echo $f | sed 's/\.\/configs//'`
   echo "cp $f -->  $new_loc"
   sudo cp $f $new_loc
   sudo chmod a+x $new_loc
done

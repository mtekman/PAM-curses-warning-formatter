#!/bin/bash

./scripts/buildInstallPamScript.sh
./scripts/buildWarner.sh
./scripts/buildOnAuth.sh

echo ""
echo "Installing into filesystem:"

for f in `find ./configs -type f ! -name "__placeholder__"`; do
   new_loc=`echo $f | sed 's/\.\/configs//'`
   #echo "cp $f -->  $new_loc"
   sudo cp -v $f $new_loc
   sudo chmod a+x $new_loc
done

echo ""
echo "[Done] Either switch to a new tty, or reboot to test."

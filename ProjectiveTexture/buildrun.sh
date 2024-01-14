#! /bin/sh
rm -rf out/build/resources
cp -R ./resources ./out/build/
cd out/build
if make ; then
    echo "Make successfully!"
    cd ../..
    ./run.sh
else
    echo "Make fail"
fi

#!/bin/bash

echo "/////////////////////////////////////////////////////////////////////"
echo -e "Making unicomm documentation...\n"

doxygen Doxyfile
cp ../sources/template/css/navtree.css ../manuals/html/
cp ../sources/template/images/body-bg.png ../manuals/html/
#!/bin/bash

# you must have the following Debian packages:
#   * php5
#   * graphviz
#   * eog
make run && php htf2dot.php out.ht out.dot && dot -Tpng out.dot -o out.png && eog out.png


#!/bin/bash
valgrind --leak-check=full --show-reachable=yes -v ./check_xml 2>&1 | tee check_xml.log

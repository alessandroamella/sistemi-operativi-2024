#!/bin/bash

sudo apt update && sudo apt install umps3 -y && make clean && mkdir -p build && tar czvf build/phase1.tar.gz AUTHORS README.md Makefile phase1/ headers/

#!/bin/bash

make -j`nproc --all` && st-flash write build/ch.bin 0x8000000

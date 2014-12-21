#!/bin/bash

(cd package/ ; R CMD build . && mv rPithon*.tar.gz .. )


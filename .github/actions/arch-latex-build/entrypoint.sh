#!/bin/sh -l

latexmk -output-format=pdf main.tex
makeglossaries main
biber main
latexmk -output-format=pdf main.tex

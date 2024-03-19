#!/bin/sh -l

latexmk -output-format=pdf main.tex
makeglossaries main
latexmk -output-format=pdf main.tex

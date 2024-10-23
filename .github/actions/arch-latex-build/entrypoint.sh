#!/bin/sh -l

latexmk -xelatex --shell-escape -output-format=pdf main.tex
biber main
makeglossaries main
latexmk -xelatex --shell-escape -output-format=pdf main.tex

#!/bin/bash
latex data_intensive_paper
bibtex data_intensive_paper
latex data_intensive_paper
latex data_intensive_paper && dvipdf data_intensive_paper.dvi && evince data_intensive_paper.pdf

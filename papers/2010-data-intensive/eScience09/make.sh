#!/bin/bash
latex data_intensive_paper
bibtex data_intensive_paper
latex data_intensive_paper
latex data_intensive_paper && evince data_intensive_paper.pdf

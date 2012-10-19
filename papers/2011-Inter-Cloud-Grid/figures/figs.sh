#!/bin/sh

fig2dev -Lpdf arch_1.fig arch_1.pdf && echo created arch_1.pdf
fig2dev -Lpdf arch_2.fig arch_2.pdf && echo created arch_2.pdf
fig2dev -Lpdf arch_3.fig arch_3.pdf && echo created arch_3.pdf
fig2dev -Lpdf arch_4.fig arch_4.pdf && echo created arch_4.pdf

fig2dev -Lpng arch_1.fig arch_1.png && echo created arch_1.png
fig2dev -Lpng arch_2.fig arch_2.png && echo created arch_2.png
fig2dev -Lpng arch_3.fig arch_3.png && echo created arch_3.png
fig2dev -Lpng arch_4.fig arch_4.png && echo created arch_4.png


perfdata <- read.table("~/Projects/saga_cactus_escience_2007/code/data/gnuplot-loni-gg.Rdata", header=TRUE)

require(gplots)

boxplot(perfdata, main="LONI/Gumbogrid Throughput Performance", col="blue", xlab="Connections", ylab="Throughput (10^6 bits/sec)")

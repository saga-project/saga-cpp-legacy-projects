library("ggplot2")

bj.singleresource <- read.csv("~/Dropbox/SAGA/papers/troy/pstar/perf/sc/bj-singleresource.txt")
new = split(bj.singleresource, bj.singleresource$LRMS.URL)

ls<-new$"sge-ssh://login2.ls4.tacc.utexas.edu"   
ls_agg <- aggregate(ls, list(ls$X.jobs), mean)
ls_agg_sd <- aggregate(ls, list(ls$X.jobs), sd)


ggplot(bj.singleresource, aes(x=X.jobs, y=(BJ.Runtime-Queuing.Time), colour=LRMS.URL)) + stat_summary(fun.y=mean, geom="line", size=3) 
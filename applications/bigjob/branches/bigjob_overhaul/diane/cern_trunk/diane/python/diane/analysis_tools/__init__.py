def read_journal(fn,select=None,MAXEVENT=0,scope={}):
    """ Read the journal file fn and return an iterator of the journal entries.
    Usage:
        for entry in read_journl(fn):
           print 'timestamp=',entry[0], 'label=',entry[1], 'keywords_dictionary=',entry[2]

    If optional select string is provided the only the lines which contain it will be read (it works like simple grep).
    This is to speed up the processing of large journals but you beware of selecting too little.
    """
    cnt = 1
    for line in file(fn):
        if MAXEVENT and cnt > MAXEVENT:
            return
        cnt += 1
        if select:
            if line.find(select) != -1:
                yield eval(line,scope)
        else:
            yield eval(line,scope)

# TODO: to be included in read_journal
class Params:
    def __init__(self,params):
        for p in params:
            setattr(self,p,params[p])

    def __str__(self):
        return str(self.__dict__)
    __repr__=__str__


# def convert2(fn1,fn2):
#     import pickle
#     f2 = file(fn2,'w')
#     for e in read_journal(fn1):
#         pickle.dump(e,f2)
# def read_journal2(fn):
#     import pickle
#     f = file(fn)
#     while 1:
#         yield pickle.load(f)

from filelog2journal import filelog2journal
from stats import Histogram1D,QuantileFinder, LogscaleHistogram1D


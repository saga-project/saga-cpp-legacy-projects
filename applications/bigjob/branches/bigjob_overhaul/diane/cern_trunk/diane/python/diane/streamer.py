
import zlib
import cPickle

#FIXME: sometimes with cPickle there is problem pickling exception objects (e.g. exceptions.AssertionError lookup)
#import pickle as cPickle

def loads(dxp_repr):
    return cPickle.loads(zlib.decompress(dxp_repr))

def dumps(obj):
    return zlib.compress(cPickle.dumps(obj))

EMPTY = dumps("")

if __name__ == "__main__":
    test_string = 'test_string'
    assert loads(dumps(test_string)) == test_string
    assert loads(EMPTY) == ""
    print "passed ok"

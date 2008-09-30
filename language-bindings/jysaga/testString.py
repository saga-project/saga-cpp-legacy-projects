import time
import array

class test(object):
    nr = 1000
    s = "abcdefghijklmnopqrstuvwxy" 
    l = ['a','b','c','d','e','f','g', 'h', 'i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y']
    a = array.array('c', "abcdefghijklmnopqrstuvwxy")
    cs = 1  #chuncksize
    
    def metString(self):
        size = len(self.s)
        for j in range(0, self.nr):
            for i in range(size - 1):
                left = self.s[0:i]
                midden = self.s[i].swapcase()
                right = self.s[i+1:size]
                self.s = left + midden + right
                self.s = self.s[0:-1] + self.s[size-1].swapcase()
            #print s

    def completeString(self):
        size = len(self.s)
        for j in range(0, self.nr):
            self.s = self.s.swapcase()
        #print s    

    def stringChunck(self):
        size = len(self.s)
        for j in range(0, self.nr):
            for i in range(0, size, self.cs):
                left = self.s[0:i]
                midden = self.s[i:i+self.cs].swapcase()
                right = self.s[i+self.cs:size]
                #print left, "_", midden, "_", right
                self.s = left + midden + right
                #self.s = self.s[0:-1] + self.s[size-1].swapcase()
            #print self.s

    def metList(self):
        size = len(self.l)
        for j in range(0, self.nr):
            for i in range(size-1):
                self.l[i] = self.l[i].swapcase()
             
    def metArray(self):
        size = len(self.a)
        for j in range(0, self.nr):
            for i in range(size-1):
                self.a[i] = self.a[i].swapcase()
            #print s    

def runner(t):
    t1 = time.time()
    t.metString()
    t2 = time.time()
    print 'metString()    took %0.3f ms' % ( (t2-t1)*1000.0)
    
    t1 = time.time()
    t.metList()
    t2 = time.time()
    print 'metList()      took %0.3f ms' % ( (t2-t1)*1000.0)
    
    t1 = time.time()
    t.metArray()
    t2 = time.time()
    print 'metArray()     took %0.3f ms' % ( (t2-t1)*1000.0)
    
    t1 = time.time()
    t.stringChunck()
    t2 = time.time()
    print 'stringChunck() took %0.3f ms' % ( (t2-t1)*1000.0)
    
t = test()
runner(t)
t.s = t.s * 10
t.l = t.l * 10
t.a = t.a * 10
t.cs = 5
runner(t)

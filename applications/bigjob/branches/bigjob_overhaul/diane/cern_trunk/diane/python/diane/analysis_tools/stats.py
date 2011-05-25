import math

#import matplotlib as mpl
#mpl.rcParams['text.usetex']=True

#FIXME: statistics assume weight=1.0 (counting bins) and are probably incorrect if weight!=1.0 is used

class Histogram1D:
    """Very simple histogramming class for quick analysis.
    Entries which go to overflow and underflow bins DO COUNT for mean and variance.
    """
    def __init__(self,a,b,n,title=''):
        self.a = float(a)
        self.b = float(b)
        self.n = n
        self.bins = [0 for x in xrange(n)]
        self.entries = [0 for x in xrange(n)]
        self.underflow = 0
        self.underflow_entries = 0
        self.overflow = 0
        self.overflow_entries = 0
        self.mean = 0
        self.qvariance = 0
        self.hits = 0
        self.title = title
        self.min = 0
        self.max = 0

    def stddev(self):
        if self.hits in [0,1]:
            return 0
        return math.sqrt(self.qvariance/(self.hits-1))
    stddev = property(stddev)
    
    def _get_index(self,x):
        return int((x-self.a)/(self.b-self.a)*self.n)

    def _get_pos(self,i,offset=0.0):
        delta = (self.b-self.a)/self.n
        return self.a+delta*(i+offset)

    def _xml_name(self):
        return "histogram1D"

    def fill(self,x,weight=1.0):
        i = self._get_index(x)
        if i < 0:
            self.underflow += weight
            self.underflow_entries += 1
        else:
            if i >=self.n:
                self.overflow += weight
                self.overflow_entries += 1
            else:
                self.bins[i] += weight
                self.entries[i] += 1

        if self.hits == 0:
            self.min = self.max = x
        else:
            self.min = min(x,self.min)
            self.max = max(x,self.max)
            
        self.hits += 1

        if self.hits == 1:
            self.mean = float(x)
            self.qvariance = 0.0
        else:
            last_mean = self.mean
            self.mean = self.mean + (x-self.mean)/self.hits
            self.qvariance = self.qvariance + (x-self.mean)*(x-last_mean)
            # another, equivalent(?) method
            #self.qvariance = self.qvariance + (self.hits-1)*(x-self.mean)*(x-self.mean)/self.hits
            #self.mean = self.mean + (x-self.mean)/self.hits

    def points(self):
        return [(self._get_pos(i,0.5), self.bins[i]) for i in xrange(self.n)]

    def xbinpos(self,pos=0.5):
        return  [ self._get_pos(i,pos) for i in xrange(self.n)]

    def printout(self,f=None):
        if f is None:
            import sys
            f = sys.stdout
        print >>f, 'Histogram1D, a=%f b=%f n=%d'%(self.a,self.b,self.n)
        print >>f, 'Title:',self.title
        inrange = reduce(lambda x,y:x+y, self.bins)
        print >>f, 'Entries: all=%d'%self.hits
        print >>f, 'Bins: inrange=%d underflow=%d overflow=%d'%(inrange, self.underflow,self.overflow)
        print >>f, 'Mean: %f StdDev: %f'%(self.mean, self.stddev)
        print >>f, 'Min: %f, Max: %f'%(self.min,self.max)
        points = self.points()
        maxp = max([p[1] for p in points])
        if maxp == 0:
            maxp=1
        for p in points:
            print >>f, "% 10.3f %5d %s"%(p[0],p[1],'#'*int(p[1]/float(maxp)*30))

    def plot(self,fmt='k-',autoscale=True,line=None,annotate=True, annotate_fmt=None, **kwds):
        "Make a matplot lib plot of the histogram outline using fmt ('k-' default) and **kwds (see help(matplotlib.pylab.plot))"

        if not annotate_fmt:
            annotate_fmt = "%.2f"

        def make_outline(en,eb,zero=0.0):
            stepSize = eb[1] - eb[0]
            binsIn = eb
            bins = [zero for i in range(len(eb)*2+2)]
            data = [zero for i in range(len(eb)*2+2)]
            for bb in range(len(binsIn)):
                bins[2*bb + 1] = binsIn[bb]
                bins[2*bb + 2] = binsIn[bb] + stepSize
                data[2*bb + 1] = en[bb] 
                data[2*bb + 2] = en[bb]
            bins[0] = bins[1]
            bins[-1] = bins[-2]
            data[0] = zero
            data[-1] = zero            
            return (bins, data)

        import matplotlib.pylab
        matplotlib.pylab.title(self.title)
 
        hbins = self.bins[:]

        # this is a workaround for matplotlib problem with drawing vertical lines to zero on logscales
        if matplotlib.pylab.gca().get_yscale() == 'log':
            zero = min([x for x in hbins if x!=0])
            print "WARNING: ZERO ON LOGSCALE, ADJUSTING DATA BY EPS=",zero 
            for i in range(len(hbins)):
                if hbins[i] == 0:
                    hbins[i] = zero
        else:
            zero = 0.0

        bins,data = make_outline(hbins,self.xbinpos(0.0),zero)
        if line:
            line.set_ydata(data)
            matplotlib.pylab.draw()
        else:
            line, = matplotlib.pylab.plot(bins,data, fmt, **kwds)
        if autoscale:
            xmargin = (self.b-self.a)/10.0
            ymin,ymax = min(hbins),max(hbins)
            ymargin = (ymax-ymin)/10.0
            matplotlib.pylab.axis([self.a-xmargin,self.b+xmargin,ymin-ymargin,ymax+ymargin])
        if annotate:            
            atxt = '$N_{ent}=%d'%self.hits 
            uo = []
            if self.underflow:
                uo.append("-%d"%self.underflow)
            if self.overflow:
                uo.append("+%d"%self.overflow)

            if uo:
                atxt += " (%s)"%",".join(uo)

            atxt += '$\n$\mu=%f, \sigma=%f$\n$min=%f, max=%f$\n'
            atxt = atxt.replace('%f',annotate_fmt)
            atxt = atxt%(self.mean,self.stddev,self.min,self.max)

            if hasattr(self,'qfinder'):
                qv = self.qfinder.quantiles.keys()
                qv.sort()
                print qv,self.qfinder.quantiles
                ql = '$q_{%d}=%f$ '.replace('%f',annotate_fmt)
                for q in qv:
                    atxt += ql%(q,self.qfinder.quantiles[q])

            ax = matplotlib.pylab.gca()

            matplotlib.pylab.text(0.95,0.95,atxt,horizontalalignment='right',verticalalignment='top',transform = ax.transAxes)
#            at = matplotlib.pylab.AnchoredText("Figure 1a",
#                              loc=2, frameon=True)
#            ax = matplotlib.pylab.get_ca()
#            ax.patch.set_boxstyle("round,pad=0.,rounding_size=0.2")
#            ax.add_artist(at)
        return line

    def createFromAidaXML(f):
        """ Parse AIDA XML file and return a dictionary of 1D histograms accessible by their names.
        """
        import dxml
        root = dxml.ParseFile(f)
        hists = {}
        for ename in root.elements['aida'][0].elements:
            print ename
            HCLS = None
            if ename == 'histogram1d':
                HCLS = Histogram1D
            if ename == 'LogscaleHistogram1D':
                HCLS = LogscaleHistogram1D
            if not HCLS:
                # ignore unknown element
                continue

            for hist in root.elements['aida'][0].elements[ename]:

                print hist
                #statistics = hist.elements['statistics'][0]
                axis = hist.elements['axis'][0]
                try:
                    title = hist['title']
                except KeyError:
                    title = ''                
                h1 = HCLS(float(axis['min']),float(axis['max']),int(axis['numberOfBins']),title=title)
                hists[hist['name']] = h1
                
                #fix by Dietrich...
                try:
                    bin1d_list = hist.elements['data1d'][0].elements['bin1d']
                except KeyError:
                    bin1d_list = []

                for bin1d in bin1d_list:
                    height = float(bin1d['height'])
                    entries = int(bin1d['entries'])
                    if bin1d['binNum'] == 'UNDERFLOW':
                        h1.undeflow = height
                        h1.underflow_entries = entries
                    else:
                        if bin1d['binNum'] == 'OVERFLOW':
                            h1.overflow = height
                            h1.overflow_entries = entries                        
                        else:
                            idx = int(bin1d['binNum'])
                            h1.bins[idx] = height
                            h1.entries[idx] = entries
                    h1.hits += entries
        return hists
    createFromAidaXML = staticmethod(createFromAidaXML)

    def storeToAidaXML(f,hists):
        print >>f, '''<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE aida SYSTEM "http://aida.freehep.org/schemas/3.2.1/aida.dtd" >
<aida version="3.2.1">
<implementation package="DIANE.stats" version="1"/>
'''
        for name in hists:
            h = hists[name]
            print >>f, '<%s name="%s" title="%s">'%(h._xml_name(),name,h.title)
            print >>f, '<axis min="%s" max="%s" numberOfBins="%s"/>'%(h.a,h.b,h.n)
            print >>f, '<data1d>'
            print >>f, '<bin1d binNum="UNDERFLOW" height="%s" entries="%s"/>'%(h.underflow,h.underflow_entries)
            print >>f, '<bin1d binNum="OVERFLOW" height="%s" entries="%s"/>'%(h.overflow,h.overflow_entries)
            for i in xrange(len(h.bins)):
                if h.bins[i] != 0:
                    print >>f, '<bin1d binNum="%i" height="%s" entries="%s"/>'%(i,h.bins[i],h.entries[i])
            print >>f, '</data1d>'
            print >>f, '</%s>'%h._xml_name()          
        print >>f, '</aida>'
    storeToAidaXML = staticmethod(storeToAidaXML)
    
    def mergeHistograms(f,sum_hists):
        """ 
        """
        hists = Histogram1D.createFromAidaXML(f)
        for name in hists:
            try:
                sum = sum_hists[name]
            except KeyError:
                sum_hists[name] = hists[name]
            else:    
                sum.addHistogram(hists[name])    
        return sum_hists
    mergeHistograms = staticmethod(mergeHistograms)

    def addHistogram(self,h):
        """+="""
        assert self.a == h.a
        assert self.b == h.b
        assert self.n == h.n
        for i in xrange(len(self.bins)):
            self.bins[i] += h.bins[i]
        self.hits += h.hits
        self.underflow += h.underflow
        self.overflow += h.overflow
        #FIXME: mean, qvariance


class QuantileFinder:
    def __init__(self):
        self.x = []
        self.quantiles = {}

    def fill(self,x):
        self.x.append(x)

    def make_quantiles(self,quantiles=[50]):
        quantiles = quantiles[:]
        quantiles.sort()
        self.x.sort()
        N = len(self.x)
        for q in quantiles:
            if N == 0:
                self.quantiles[q] = 0
                continue
            qi = (N-1)*q/100.0
            i = int(qi)
            #print q,i,qi,N
            if qi == i or i==N-1:
                self.quantiles[q] = self.x[i]
            else:
                self.quantiles[q] = (self.x[i]+self.x[i+1])/2.0

        # release the memory
        self.x = []
        return self.quantiles


# TODO: FIXME: plotting of LoscaleHistogram is currently broken!

class LogscaleHistogram1D(Histogram1D):
    """1D Histogram with bins in a logarithmic scale"""
    def __init__(self,*args,**kwds):
        Histogram1D.__init__(self,*args,**kwds)
        self.vv = math.pow((self.b-self.a),1.0/self.n)

    def _get_index(self,x):
        if x-self.a < 0:
            return -1
        else:
            return int(math.log(x-self.a,self.vv))

    def _get_pos(self,i,offset=0.0):
        return self.a + math.pow(self.vv,i+offset)
            
    def _xml_name(self):
        return "LogscaleHistogram1D"


if __name__ == "__main__":
    q=QuantileFinder()
    l=[700,100,200,300,400,500,600]
    for x in range(100):
        q.fill(x)

    print q.make_quantiles([50,95,5])
        
def opts(**kwds):
    return kwds

def barplot(labels,data,error=None,bar_opts={},ticks_opts={}):
    from matplotlib.pylab import bar,yticks,xticks,xlim
    xlocations = [0.5+x for x in range(len(data))]
    bar_opts.setdefault('width',0.5)
    bar(xlocations, data, yerr=error, **bar_opts)
    xticks([bar_opts['width']/2.0+x for x in xlocations], labels, **ticks_opts)
    xlim(0, xlocations[-1]+1)
    return xlocations


def errbar_labels(labels,data,error,bar_opts={},ticks_opts={}):
    from matplotlib.pylab import plot,xlim
    bar_opts.update(opts(width=0,linewidth=0, fill=0))
    xlocations=barplot(labels,data,error,bar_opts,ticks_opts)
    plot(xlocations,data,"or")
    xlim(0, xlocations[-1]+0.5)
    return xlocations
    

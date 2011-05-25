# minimal DOM-like parser based on expat
# does not cover full XML but enough for many XML data representations

import xml.parsers.expat

class Element:
    def __init__(self,name,attrs={}):
        self.name = name
        self.attrs = attrs
        self.elements = {}

    def __getitem__(self,a):
        return self.attrs[a]

    def printOut(self,level=0):
        space = ' '*level
        if self.name:
            print '%s<%s attrs=%s>'%(space,self.name,self.attrs)

        for element_list in self.elements.values():
            for element in element_list:
                element.printOut(level+1)
        if self.name:
            print '%s</%s>'%(space,self.name)

def ParseFile(f):
    p,root = _make_parser()
    p.ParseFile(f)
    return root

def _make_parser():
    root = Element('')
    stack = [root]

    def start_element(name, attrs):
        e = Element(name,attrs)
        el = stack[-1].elements.setdefault(name,[])
        el.append(e)
        stack.append(e)

    def end_element(name):
        e = stack.pop()
        assert e.name == name

    p = xml.parsers.expat.ParserCreate()
    p.StartElementHandler = start_element
    p.EndElementHandler = end_element

    return p,root

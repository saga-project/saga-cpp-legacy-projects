############### config ##################

import diane.config
import os

if os.environ.has_key('DIANE_CONFIG'):
    del os.environ['DIANE_CONFIG']
    
diane.config.load_config_file()
c = diane.config.getConfig(None)

print '## @page ConfigurationOptions Configuration options'
print '#<ul>'
for s in c:
    print '# <li><b>',s.getName(),'</b><ul>'
    
    for o in s:
        print '# <li><b>%s</b>. <ul><li> %s. <li> Default value: %s</ul>'%(o.name,o.doc,o.value)
        
    print '#</ul></li>'
    
print '#</ul>'
print

############### commands ##################

import glob,os.path

print '## @page CommandsIndex Commands Index'
print '#@par Command summary'
print '#<ul>'
clist = [c for c in glob.glob('../bin/*') if c.find('~') == -1 and c.find('CVS')==-1]
clist.sort()
for c in clist:
    bc = os.path.basename(c)
    print '##<li>@ref',bc,'</li>'

print '#</ul>'

print '#@par Command details'
import subprocess

for c in clist:
    bc = os.path.basename(c)
    print '##@section',bc,bc
    print '#@verbatim'
    for l in subprocess.Popen([c, "-h"], stdout=subprocess.PIPE,stderr=subprocess.STDOUT).communicate()[0].splitlines():
        print '#',l
    print '#@endverbatim'

print

## ############### environment variables  ##################

## print  '''
## ## @page EnvironmentVariables Environment Variables
## #Here are the environment variables'''

## import diane.util.env
## vars = diane.util.env.vars

## for name in vars:
##     v = vars[name]
##     print '''#@par %(name)s:
## #<ul>
## #<li> %(doc)s
## #<li> default value: \b%(defvalue)s'''%v.__dict__
##     if v.values:
##         print '#<li> other values:\n#<ul>'
##     for vv in v.values:
##         print '#<li> %s </li>'%str(vv)
##     print '#</ul></ul>'

## print


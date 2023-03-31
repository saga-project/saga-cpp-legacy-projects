#!/usr/bin/env python

import sys
import bigdata
from setuptools import setup, find_packages

try:
    import saga
except:
    print "SAGA and SAGA Python Bindings not found: Please install SAGA first (http://saga.cct.lsu.edu)."
    #sys.exit(1)
    
setup(name='BigData',
      version=bigdata.version,
      description='SAGA-based Pilot-Data Implementation',
      author='Andre Luckow',
      author_email='aluckow@cct.lsu.edu',
      url='http://faust.cct.lsu.edu/trac/bigjob',
      classifiers = ['Development Status :: 4 - Beta',                    
                    'Programming Language :: Python',
                    'Environment :: Console',                    
                    'Topic :: Utilities',
                    ],
      platforms = ('Unix', 'Linux', 'Mac OS'),
      packages=['bigdata', 'examples'],
      data_files=['bigdata/VERSION'],
      install_requires=['uuid', 'threadpool', 'virtualenv', 'redis==4.5.4', 'paramiko'],
      entry_points = {
        'console_scripts': []
      }
)

#!/usr/bin/env python

#from distutils.core import setup
import sys
from setuptools import setup, find_packages

try:
    import saga
except:
    print "SAGA and SAGA Python Bindings not found: Please install SAGA first (http://saga.cct.lsu.edu)."
    #sys.exit(1)
    
setup(name='BigData',
      version='0.1.0',
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
      data_files=[],
      install_requires=['uuid', 'threadpool', 'virtualenv', 'redis==2.2.4', 'paramiko'],
      entry_points = {
        'console_scripts': []
      }
)

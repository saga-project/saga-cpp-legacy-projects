#!/bin/bash

rm ~/.bigjob/python
virtualenv ~/.bigjob/python
. ~/.bigjob/python/bin/activate

easy_install bigjob

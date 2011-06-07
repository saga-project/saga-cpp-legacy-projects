#!/bin/sh

BIGJOB_HOME=/home/marksant/proj/bigjob/branches/bigjob_overhaul
PYTHONPATH=$PYTHONPATH:$BIGJOB_HOME/diane
diane-run $BIGJOB_HOME/diane/bigjob_diane_backend/bigjob_diane_backend.run &

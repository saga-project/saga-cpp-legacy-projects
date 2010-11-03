#!/bin/sh

source /home/lukasz/saga/1/.env

export _NMI_SAGA_TAG=$2
cd /home/lukasz/saga/1
nmi_submit $1

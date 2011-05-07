#!/bin/sh

JOBSERVICE_URL=$1
FILESYSTEM_URL=$2
DIANE_WORKER_NUMBER=$3

if [ -z "${JOBSERVICE_URL}" -o -z "${FILESYSTEM_URL}" -o -z "${DIANE_WORKER_NUMBER}" ]; then
    echo "usage: submit_worker.sh <jobservice_url> <filesystem_url> <diane_worker_number>"
    exit 1
fi

diane-submitter SAGA \
--jobservice-url=${JOBSERVICE_URL} \
--filesystem-url=${FILESYSTEM_URL} \
--diane-worker-number=${DIANE_WORKER_NUMBER}

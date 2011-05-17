#!/bin/sh

JOBSERVICE_URL=$1
FILESYSTEM_URL=$2
NUMBER_NODES=$3
WORKERS_PER_NODE=$4

if [ -z "${JOBSERVICE_URL}" -o -z "${FILESYSTEM_URL}" -o -z \
    "${NUMBER_NODES}" -o -z "${WORKERS_PER_NODE}" ]; then
    echo "usage: submit_worker.sh <jobservice_url> <filesystem_url> \
<number_nodes> <workers_per_node>"
    exit 1
fi

diane-submitter SAGA \
--jobservice-url=${JOBSERVICE_URL} \
--filesystem-url=${FILESYSTEM_URL} \
--diane-number-nodes=${NUMBER_NODES} \
--diane-workers-per-node=${WORKERS_PER_NODE}

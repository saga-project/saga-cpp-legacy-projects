#ifndef INCLUDE_BQP_VGRADS_API_H
#define INCLUDE_BQP_VGRADS_API_H

#include <bqp_data.h>


int vgrads_cluster_map(char *cluster_name, char *out_cluster_name, char *out_queue);

double bqp_query(char *cluster_name, int nodes, int reqtime, unsigned int deadline);

int bqp_reserve(char *cluster_name, int nodes, int reqtime, int deadline, double prob);

int bqp_reserve_check(int id);

int bqp_reserve_v2(char *cluster_name, int nodes, int reqtime, int deadline, double prob);

int bqp_reserve_check_v2(char *cluster_name, int nodes, int reqtime, int deadline, double prob);

void bqp_init();

int bqp_alternative(char *cluster_name, int nodes, int reqtime, int deadline, double prob, int *alt_nodes, int *alt_reqtime, int *alt_deadline, double *alt_prob);

int bqp_precog(char *cluster_name, int nodes, int reqtime, int deadline, double prob, int *outwait, int *outrtime, double *outprob);

void bqp_exit(int);

#endif




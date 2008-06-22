#ifndef INCLUDE_BQP_TOOLS_H
#define INCLUDE_BQP_TOOLS_H

#ifdef __cplusplus                                                      
extern "C" {                                                            
#endif   

  int varq_reserve(char *cluster_name, char *queue, int nodes, int reqtime, int start_deadline, double min_success_prob);
  int varq_reserve_block(char *cluster_name, char *queue, int nodes, int reqtime, int start_deadline, double min_success_prob, int *outwait, int *outrtime, double *outprob);
  int varq_request(char *cluster_name, char *queue, int nodes, int reqtime, int start_deadline, double min_success_prob, int *outwait, int *outrtime, double *outprob);

#ifdef __cplusplus                                                      
}                                                                       
#endif   

#endif


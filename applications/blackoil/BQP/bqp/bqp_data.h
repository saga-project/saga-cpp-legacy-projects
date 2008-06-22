#ifndef INCLUDE_BQP_DATA_H
#define INCLUDE_BQP_DATA_H

#ifdef __cplusplus                                                      
extern "C" {                                                            
#endif   
  
#define HTTPHOST "batchq.cs.ucsb.edu"
  //#define HTTPHOST "www.uc.teragrid.org"
#define HTTPPORT 80
#define SIZE 3

#ifndef DOCACHE
#define DOCACHE 0
#endif

  typedef struct bqp_res_t {
    char resource[32];
    char queues[256][256];
    int numqueues;
    int qidx, default_qidx;
  } bqp_res;
  
  typedef struct bqp_t {
    bqp_res *res;
    int range_min, range_max, nodes, size, numvals, reqtime;
    unsigned int ts;
    float preds[SIZE], emps[SIZE], accs[SIZE];
    char quantiles[SIZE][4];
    char confs[SIZE][4];
  } bqp;
  
  typedef struct winner_t {
    double num;
    char str[256];
  } winner;
  
  int sort_by_num_inv(const void *, const void *);
  int sort_by_num(const void *, const void *);

  //user interface
  int bqp_get_prediction(char *, char *, int, int, double, double *);
  int bqp_get_resources(bqp_res **, int *);
  int bqp_get_inverted(char *, char *, int, int, int insec, double, double *);
  int bqp_print_all_predictions(int, char *, char *, int, int, double, double);
  int bqp_print_all_predictions_internal(int, char *, char *, int, int, double, double, winner *, int *);
  int bqp_get_sdsc_predictions(int, char *, char *, int, int, double, double);
  int bqp_get_best_predictions();

  //internal
  int get_http_file(char *hostname, char *file, char **result);  
  void *Malloc(size_t);
  void Free(void *);
  
#ifdef __cplusplus                                                      
}                                                                       
#endif   

#endif

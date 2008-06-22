#include <stdio.h>
#include <stdlib.h>
#include <bqp_data.h>

/* 
   This program is meant to illustrate how the batch queue prediction
   (BQP) API is used!

   In addition to this program, some other utilities are included in
   the package which do interesting things:
   
   bqp_api_example - this program!  gives examples of fundamental bqp
   API calls without surrounding code confusion (cmdline arguments,
   rigorous error checking, formatted output, etc)

   bqpfull - gives formatted prediction information about all
   machine/queue combos we have access to (see man page for usage
   information).  exercises full bqp interface.

   bqp_precog - answers the question 'how long do i have to wait, and
   how big should my job be, in order to guarantee with probability P
   that my resources will be available from time T to T1?'
   
*/

main() {
  int rc, i, j, numres;
  double prediction;

  /* bqp_res is a struct (see bqp.h) which defines a 'resource' for the api. */
  bqp_res *theres;

  /* the first thing we may want to do is get a list of supported machines and queues.  after bqp_get_resources completes, 'theres' will be an array of bqp_res structs */

  /* 
     one you select a machine and queue combo, you can make bound
     predictions for a single job submitted to that machine/queue.
     the bound prediction returned is valid if the job described was
     submitted at the time the prediction is made (or more
     realistically, shortly thereafter).  Currently, the 0.95, 0.75,
     and 0.5 (median) bounds are the only quantiles we support through
     the API.
  */

  rc = bqp_get_prediction("ucteragrid", "dque", 1, 3600, 0.95, &prediction);
  if (rc > 0) {
    printf(".95 Prediction: %f\n", prediction);
  }
  rc = bqp_get_prediction("ucteragrid", "dque", 1, 3600, 0.75, &prediction);
  if (rc > 0) {
    printf(".75 Prediction: %f\n", prediction);
  }
  rc = bqp_get_prediction("ucteragrid", "dque", 1, 3600, 0.50, &prediction);
  if (rc > 0) {
    printf(".50 Prediction: %f\n", prediction);
  }

  /*
    The next routine can be thought of as an 'inverted prediction'
    when compared with the one above.  This function answers the
    question 'with what probability will job X execute on or before
    deadline Y?', where 'Y' in the following example is 43200 seconds
    (1/2 a day)
  */
  rc = bqp_get_inverted("bigben", "debug", 2, 2500, 43200, 0, &prediction);
  if (rc > 0) {
    printf("Probability of job running at or before deadline: %f\n", prediction);
  }
  
}

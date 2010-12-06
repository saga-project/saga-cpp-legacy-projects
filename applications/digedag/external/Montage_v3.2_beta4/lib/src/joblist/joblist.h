#ifndef JOBLIST_H
#define JOBLIST_H

#define JL_MAXSTR 4096

#define JL_OK        0
#define JL_ERROR    -1
#define JL_EOF      -2

extern char *jl_status[];

extern int  jlDebug    (FILE *debug);
extern int  jlJobFile  (char *jobfile, char *lockfile);
extern int  jlGetJob   (char *jobid);
extern int  jlSetStatus(char *statusStr, char *returnStr);

#endif


# this file is not in the automatic testing framework
# you may test it by hand using LocalSubmitter.py
#
# start master:
#  - diane-run doctor_test.py &
# submit worker:
#  - diane-env -d ganga LocalSubmitter.py --diane-run-file doctor_test.py --diane-platform-doctor=1

import diane_test_applications.idle as application

from diane.submitters import sh_download_platform_doctor_wrapper as w

# make sure we use the download wrapper with doctor...
# and that Ganga does not remove the /tmp dir so that we can inspect the files
def initialize_submitter(s):
   s.wrapper = w
   from Ganga.GPI import config
   config.Local.remove_workdir = False

def run(input,config):
   pass

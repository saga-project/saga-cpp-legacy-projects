""" Module API
This Module contains the API of the DARE framework
"""

class dare(object):

    def __init__():
         pass

    def launch_resources_job(self,resource_list)
        pass

    def calculate_nodes(self):
        pass

    def file_stage(self,source_url, dest_url):
        pass

    def subjob_submit(self, jd_executable, jd_number_of_processes, jd_spmd_variation,\
                     jd_arguments, affinity, jd_work_dir, jd_output, jd_error):
        pass
        
    def read_conf(self, filename):       
        pass
    
    def wait_for_subjobs()
    
    #for subjob state
    def has_finished(state):
        state = state.lower()
        if state=="done" or state=="failed" or state=="canceled":
            return True
        else:
            return False

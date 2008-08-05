import saga

def submit_job():
        js = saga.cpr.service(saga.url("migol://qb1.loni.org/jobmanager-pbs"))
        jd = saga.cpr.description()
        jd.executable="/bin/date"
        namd_job = js.create_job(jd,jd)
        namd_job.run()

if __name__ == "__main__" :
        submit_job()


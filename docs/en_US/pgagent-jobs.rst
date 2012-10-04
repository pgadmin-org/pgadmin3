.. _pgagent-jobs:


*********************
`pgAgent Jobs`:index:
*********************

pgAgent runs 'jobs', each of which consists of :ref:`pgagent-steps` and 
:ref:`pgagent-schedules`. To configure jobs on your
server, browse to the server on which the pgAgent database objects
were created. A *Jobs* node will be displayed, under 
which individual jobs are shown. New jobs may be created, and existing
jobs modified in the same way as any other object in pgAdmin.

.. image:: images/pgagent-jobdetails.png

The properties tab in the main pgAdmin window will display the 
details of the selected job, and the Statistics tab will show the 
details of each run of the job.

.. image:: images/pgagent-jobstats.png

Job themselves are very simple - the complexity is in the steps and
schedules. When configuring a job, we specify the job name, whether or
not it is enabled, a job class or category, and optionally the hostname
of a specific machine running pgAgent, if this job should only run on
that specific server. If left empty (which is normally the case), any 
server may run the job. This is not normally needed for SQL-only jobs, 
however any jobs with batch/shell steps may need to be targetted to a 
specific server. A comment may also be included.

**Note:** It is not always obvious what value to specify for the
Host Agent in order to target a job step to a specific machine. With pgAgent
running on the required machines and connected to the scheduler database, 
the following query may be run to view the hostnames as reported by each
agent::

  SELECT jagstation FROM pgagent.pga_jobagent

Use the hostname exactly as reported by the query in the Host Agent field.

.. image:: images/pgagent-jobproperties.png


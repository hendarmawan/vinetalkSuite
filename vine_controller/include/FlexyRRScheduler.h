/*
 * Copyright 2018 Foundation for Research and Technology - Hellas
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0 [1] [1]
 *
 * Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 *  implied.
 * See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * Links:
 *  ------
 * [1] http://www.apache.org/licenses/LICENSE-2.0 [1] 
*/
#ifndef VINE_ROUND_ROBIN_SCHEDULER
#define VINE_ROUND_ROBIN_SCHEDULER
#include "Scheduler.h"
#include <map>
#include <vector>
#include <atomic>

/*
 * FlexyRRScheduler:
 * One scheduler to rule them all
 * Its flexible and RoundRobin.
 */
class FlexyRRScheduler : public Scheduler {
    public:
        /*
		 * Constructor.
		 *
		 * args contains of comma separated key:value pairs in the following
		 * format:
		 *
		 * key1:val1,key2:val2
		 *
		 * Acceptable Keys:
		 * elastic:true|false	// Enable/disable elastic mode
		 * sla:<file>			// Use <file> to load SLA and other settings for jobs
		 */
        FlexyRRScheduler(std::string args);
        virtual ~FlexyRRScheduler();
        /*Select a task from all the VAQs that exist in the system  */
        virtual vine_task_msg_s *selectTask( accelThread *threadPerAccel );
		virtual void postTaskExecution(vine_task_msg_s *task);

    private:
		int userRR;	// Round Robin Counter for User Facing Tasks
		int batchRR;	// Round Robin Counter for Batch Tasks
		bool elastic;	// True if args contains elastic:true
};

#define JOB_METRIC_SAMPLES 16

class JobMetrics
{
	private:
		uint64_t counter;
		double durations[JOB_METRIC_SAMPLES];
		double cumulative_task_ttc;	// Time To Completion
		double sla;					// Sla in us
		int over_sla_threshold;
		int under_sla_threshold;
		int over_sla;
		int under_sla;

	public:
		friend std::ostream & operator<<(std::ostream & os,const JobMetrics & jm);
		JobMetrics(double sla = 600000,double over_sla_threshold = 10,double under_sla_threshold = 10) :
		counter(0), cumulative_task_ttc(0), sla(sla),
		over_sla_threshold(over_sla_threshold), under_sla_threshold(under_sla_threshold),
		over_sla(0), under_sla(0)
		{
			std::fill(durations,durations+JOB_METRIC_SAMPLES,0);
		}

		void addDuration(double dur)
		{
			uint64_t index = __sync_add_and_fetch(&counter,1);
			index %= JOB_METRIC_SAMPLES;
			cumulative_task_ttc -= durations[index];
			durations[index] = dur;
			cumulative_task_ttc += dur;
			if(dur > sla)
			{
				__sync_add_and_fetch(&over_sla,1);
				under_sla = 0;
			}
			else
			if(dur < sla)
			{
				__sync_add_and_fetch(&under_sla,1);
				over_sla = 0;
			}
		}

		bool overSLA()
		{
			return over_sla >= over_sla_threshold;
		}

		bool underSLA()
		{
			return under_sla >= under_sla_threshold;
		}

		double getAverageDuration() const
		{
			if(counter < JOB_METRIC_SAMPLES)
				return cumulative_task_ttc/counter;
			else
				return cumulative_task_ttc/JOB_METRIC_SAMPLES;
		}
};

std::ostream & operator<<(std::ostream & os,const JobMetrics & jm);

#endif

#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <climits>
#include <cfloat>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <string>
#include <vector>
#include <array>
#include <iterator>
#include <set>
#include <list>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <stack>
#include <deque>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <stdexcept>
#include <condition_variable>
#include <pthread.h>
#include <semaphore.h>
#include "progtest_solver.h"
#include "sample_tester.h"
using namespace std;
#endif /* __PROGTEST__ */ 

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

/**
 * COptimizer: A class responsible for orchestrating the entire optimization process.
 * It manages the communicators and workers to ensure efficient problem-solving.
 */
class COptimizer
{
	public:
		COptimizer ( )
		: m_Communicators ( ), m_Workers ( ), m_Solver (nullptr), m_RBuffer ( ), m_PBuffer ( ),
		m_RMutex (make_shared <mutex> ( )), m_PMutex (make_shared <mutex> ( )), m_SMutex (make_shared <mutex> ( )), m_SCMutex (make_shared <mutex> ( )),
		m_WCondVar (make_shared <condition_variable> ( )), m_StartedCompanies (0), m_StoppedCompanies (0) { }

		/**
 		 * Indicates if the ProgtestSolver is being used.
 		 *
 		 * @return True if using ProgtestSolver, otherwise false.
 		 */
		static bool usingProgtestSolver ( ) { return true; }

		/**
		 * Dummy implementation to check the algorithm.
		 *
		 * @param problem The problem to check.
		 */
        static void checkAlgorithm ( AProblem problem ) { } // dummy implementation if usingProgtestSolver() returns true

		/**
		 * Starts the optimization process by launching the worker threads.
		 *
		 * @param worker_count Number of worker threads to launch.
		 */
        void start ( int worker_count )
        {
			for (size_t worker_ID = 0; worker_ID < size_t (worker_count); worker_ID++) { AWorker worker = make_shared <CWorker> ( ); m_Workers.push_back (worker); }

	        for (auto & communicator : m_Communicators) communicator->launch (m_RBuffer, m_RMutex, m_StoppedCompanies, m_SCMutex, m_WCondVar);
			for (auto & worker : m_Workers) worker->launch (ref (m_RBuffer), ref (m_RMutex), ref (m_PBuffer), ref (m_PMutex), ref (m_Solver),
			                                                 ref (m_SMutex), ref (m_WCondVar), ref (m_StartedCompanies), ref (m_StoppedCompanies), ref (m_Communicators) );
		}

		/**
		 * Stops the optimization process by joining the worker and communicator threads.
		 */
        void stop ( )
		{
			for (auto & communicator : m_Communicators) { communicator->m_Receiver.join (); communicator->m_Sender.join (); }
			for (auto & worker : m_Workers) worker->m_Worker.join ();
		}

		/**
		 * Adds a new company to the optimizer.
		 *
		 * @param company The company to add.
		 */
        void addCompany ( ACompany company )
		{
			size_t company_ID = m_Communicators.size ();
			ACommunicator communicator = make_shared <CCommunicator> (company, company_ID);
			m_Communicators.push_back (communicator); ++m_StartedCompanies;
		}

    private:
		using AWrappedPack = std::shared_ptr <pair <pair <size_t, AProblemPack>, pair <size_t, bool>>>; // <company ID, pack>, <solved problems count, is solved>
		using Buffer = list <AWrappedPack>;
		using AMutex = std::shared_ptr <mutex>; using ACondVar = std::shared_ptr <condition_variable>;

		class CCommunicator; using ACommunicator = std::shared_ptr <CCommunicator>;
		class CWorker; using AWorker = std::shared_ptr <CWorker>;

		/**
		 * CCommunicator: A class responsible for handling communication between the company and the optimizer.
		 */
		class CCommunicator
		{
			public:

				/**
				 * Constructor to initialize the CCommunicator.
				 *
				 * @param company The company associated with this communicator.
				 * @param ID The ID of the company.
				 */
				CCommunicator ( ACompany company, size_t ID ) : m_CCondVar (make_shared <condition_variable> ( )),
				m_CMutex (make_shared <mutex> ( )), m_Company (move (company)), m_ID (ID), m_CBuffer ( ), m_StoppedReceiving (false) { }

				/**
				 * Launches the receiver and sender threads for the communicator.
				 *
				 * @param g_r_buffer Global receiving buffer.
				 * @param g_r_mutex Global receiving buffer mutex.
				 * @param stopped_companies Number of stopped companies.
				 * @param g_sc_mutex Mutex for stopped companies count.
				 * @param g_w_condvar Worker condition variable.
				 */
				void launch ( Buffer & g_r_buffer, AMutex & g_r_mutex, size_t & stopped_companies, AMutex & g_sc_mutex, ACondVar & g_w_condvar )
				{
					m_Receiver = thread (& CCommunicator::receivePack, this, ref (g_r_buffer), ref (g_r_mutex), ref (stopped_companies), ref (g_sc_mutex), ref (g_w_condvar));
					m_Sender = thread (& CCommunicator::sendPack, this);
				}

				ACondVar m_CCondVar; // own sending company condition variable.
				AMutex m_CMutex; // own sending company mutex.
				thread m_Receiver, m_Sender; // communicator threads.

			private:
				/**
				 * Receives a pack from the company and places it in the global and local buffers.
				 *
				 * @param g_r_buffer Global receiving buffer.
				 * @param g_r_mutex Global receiving buffer mutex.
				 * @param stopped_companies Number of stopped companies.
				 * @param g_sc_mutex Mutex for stopped companies count.
				 * @param g_w_condvar Worker condition variable.
				 */
				void receivePack ( Buffer & g_r_buffer, AMutex & g_r_mutex, size_t & stopped_companies, AMutex & g_sc_mutex, ACondVar & g_w_condvar )
				{
					while (!(m_StoppedReceiving))
					{
						AProblemPack pack = m_Company->waitForPack ();
						if (!pack)
						{
							m_StoppedReceiving = true;  // received nullptr == company has died.
							{ unique_lock <mutex> lock (* g_sc_mutex); ++stopped_companies; g_w_condvar->notify_one (); } // notifying some worker.
							break;
						}

						AWrappedPack wrapped_pack = make_shared <pair <pair <size_t, AProblemPack>, pair <size_t, bool>>> (make_pair (make_pair (m_ID, pack), make_pair (0, false)));
						{ unique_lock <mutex> lock (* m_CMutex); m_CBuffer.push_back (wrapped_pack); } // pushing received pack to the own buffer.
						{ unique_lock <mutex> lock (* g_r_mutex); g_r_buffer.push_back (wrapped_pack); } // pushing received pack to the global buffer.
						g_w_condvar->notify_one (); // notifying some worker.
					}
				}

				/**
				 * Sends solved packs back to the company.
				 */
				void sendPack ( )
				{
					unique_lock <mutex> lock (* m_CMutex); // locking own receiving buffer.

					while (!(m_StoppedReceiving && m_CBuffer.empty ()))
					{
						m_CCondVar->wait (lock); // if sender is notifying, check if receiving buffer is not empty and the first pack is solved.

						while (!(m_CBuffer.empty ()))
						{
							AWrappedPack wrapped_pack = m_CBuffer.front ();
							if (wrapped_pack->second.second) { m_Company->solvedPack (wrapped_pack->first.second); m_CBuffer.pop_front (); }  // while packs from receiving buffer are solved we delete them (bool value is true).
							else break;
						}
					}
				}

				ACompany m_Company; // CCompany.
				size_t m_ID; // ID of CCompany to distinguish between companies.
				Buffer m_CBuffer; // own receiving buffer for handling the packs order.
				bool m_StoppedReceiving; // if CCompany stopped sending packs.

		};

		/**
		 * CWorker: A class responsible for processing and solving problems from the buffer.
		 */
		class CWorker
		{
			public:
				CWorker ( ) : m_WPBuffer ( ), m_WSolver (nullptr), m_StartedSolving (false), m_StoppedWorking (false) { }

				/**
				 * Launches the worker thread.
				 *
				 * @param g_r_buffer Global receiving buffer.
				 * @param g_r_mutex Global receiving buffer mutex.
				 * @param g_p_buffer Global processing buffer.
				 * @param g_p_mutex Global processing buffer mutex.
				 * @param g_solver Global solver.
				 * @param g_s_mutex Global solver mutex.
				 * @param g_w_condvar Worker condition variable.
				 * @param started_companies Number of started companies.
				 * @param stopped_companies Number of stopped companies.
				 * @param communicators Vector of communicators.
				 */
				void launch ( Buffer & g_r_buffer, AMutex & g_r_mutex, Buffer & g_p_buffer, AMutex & g_p_mutex, AProgtestSolver & g_solver,
				              AMutex & g_s_mutex, ACondVar & g_w_condvar, size_t & started_companies, size_t & stopped_companies, vector <ACommunicator> & communicators )
				{
					m_Worker = thread (& CWorker::work, this, ref (g_r_buffer), ref (g_r_mutex), ref (g_p_buffer), ref (g_p_mutex),ref (g_solver),
									   ref (g_s_mutex), ref (g_w_condvar), ref (started_companies), ref (stopped_companies), ref (communicators));
				}

				thread m_Worker;

			private:
				/**
				 * The main function for the worker thread to process and solve problems.
				 *
				 * @param g_r_buffer Global receiving buffer.
				 * @param g_r_mutex Global receiving buffer mutex.
				 * @param g_p_buffer Global processing buffer.
				 * @param g_p_mutex Global processing buffer mutex.
				 * @param g_solver Global solver.
				 * @param g_s_mutex Global solver mutex.
				 * @param g_w_condvar Worker condition variable.
				 * @param started_companies Number of started companies.
				 * @param stopped_companies Number of stopped companies.
				 * @param communicators Vector of communicators.
				 */
				void work ( Buffer & g_r_buffer, AMutex & g_r_mutex, Buffer & g_p_buffer, AMutex & g_p_mutex, AProgtestSolver & g_solver,
				            AMutex & g_s_mutex, ACondVar & g_w_condvar, size_t & started_companies, size_t & stopped_companies, vector <ACommunicator> & communicators )
				{
					while (!m_StoppedWorking)
					{
						fillSolver (g_r_buffer, g_r_mutex, g_p_buffer, g_p_mutex, g_solver, g_s_mutex, g_w_condvar, started_companies, stopped_companies);
						if (m_StartedSolving) solveSolver (communicators);
					}
				}

				/**
				 * Fills the solver with problems from the buffer.
				 *
				 * @param g_r_buffer Global receiving buffer.
				 * @param g_r_mutex Global receiving buffer mutex.
				 * @param g_p_buffer Global processing buffer.
				 * @param g_p_mutex Global processing buffer mutex.
				 * @param g_solver Global solver.
				 * @param g_s_mutex Global solver mutex.
				 * @param g_w_condvar Worker condition variable.
				 * @param started_companies Number of started companies.
				 * @param stopped_companies Number of stopped companies.
				 */
				void fillSolver ( Buffer & g_r_buffer, AMutex & g_r_mutex, Buffer & g_p_buffer, AMutex & g_p_mutex, AProgtestSolver & g_solver,
								  AMutex & g_s_mutex, ACondVar & g_w_condvar, size_t & started_companies, size_t & stopped_companies )
				{
					while (!m_StartedSolving)
					{
						unique_lock <mutex> lock_r_buffer (* g_r_mutex);
						while (g_r_buffer.empty () && started_companies != stopped_companies) g_w_condvar->wait (lock_r_buffer); // get out of sleep if worker was notified and general receiving buffer is not empty, or it is empty and all companies stopped.

						{
							unique_lock <mutex> lock_p_buffer (* g_p_mutex);
							if (g_r_buffer.empty () && g_p_buffer.empty () && started_companies == stopped_companies) { m_StoppedWorking = true; break; }
						}

						{ // locking solver.
							unique_lock <mutex> lock_g_solver (* g_s_mutex); // locking general solver.
							if (!g_solver) g_solver = createProgtestSolver ();

							auto filling_pack = g_r_buffer.begin (); // iterator to the used pack.
							while (g_solver->hasFreeCapacity () && filling_pack != g_r_buffer.end ()) // filling one by one pack into the solver.
							{
								size_t problem_index = (* filling_pack)->second.first;
								g_solver->addProblem ((* filling_pack)->first.second->m_Problems.at (problem_index)); ++((* filling_pack)->second.first); // filling 1 problem to the solver and saving count of used problems from pack.
								problem_index = (* filling_pack)->second.first;

								if (problem_index == (* filling_pack)->first.second->m_Problems.size ()) // this pack was whole used for filling, and now we will push it to the general processing buffer.
								{
									AWrappedPack used_pack = g_r_buffer.front (); g_r_buffer.pop_front (); filling_pack = g_r_buffer.begin (); // we take new filling pack from the general receiving buffer.
									{ unique_lock <mutex> lock_p_buffer (* g_p_mutex); g_p_buffer.push_back (used_pack); }
								}
							}

							if (g_solver->hasFreeCapacity ())
							{
								if (g_r_buffer.empty () && started_companies != stopped_companies) continue; // general receiving buffer didn't fill the solver, worker will wait for a new pack.
								else // general solver is empty, and it was last pack, all companies stopped.
								{
									{ unique_lock <mutex> lock_p_buffer (* g_p_mutex); m_WPBuffer = move (g_p_buffer); g_p_buffer = list <AWrappedPack> ( ); } // copying to the own buffer general processing buffer.
									m_WSolver = move (g_solver); g_solver = nullptr; m_StartedSolving = true; // copying to the local solver and starting to solve it.
									m_StoppedWorking = true;
								}
							}
							else
							{
								{ unique_lock <mutex> lock_p_buffer (* g_p_mutex); m_WPBuffer = move (g_p_buffer); g_p_buffer = list <AWrappedPack> ( ); } // copying to the own buffer general processing buffer.
								m_WSolver = move (g_solver); g_solver = nullptr; m_StartedSolving = true;

								if (g_r_buffer.empty () && started_companies == stopped_companies) m_StoppedWorking = true; // checking if it was last pack.
							} // copying to the local solver and setting general solver to the nullptr, it is needed to create a new one.
						}
					}

					g_w_condvar->notify_one (); // notifying another worker to start working or to get out of sleep and stop working.
				}

				/**
				 * Solves the problems using the solver.
				 *
				 * @param communicators Vector of communicators.
				 */
				void solveSolver ( vector <ACommunicator> & communicators )
				{
					m_WSolver->solve (); // solving solver.

					while (!(m_WPBuffer.empty ()))
					{
						size_t company_ID = m_WPBuffer.front ()->first.first;
						ACondVar company_condvar = communicators[company_ID]->m_CCondVar;

						m_WPBuffer.front ()->second.second = true;
						m_WPBuffer.pop_front (); company_condvar->notify_one (); // changing status to solved (true).
					}

					m_StartedSolving = false; // ended solving solver.
				}

				Buffer m_WPBuffer; // general receiving buffer, general processing buffer, own worker processing buffer.
				AProgtestSolver m_WSolver; // own worker solver.
				bool m_StartedSolving, m_StoppedWorking; // if worker started solving solver.

		};

		vector <ACommunicator> m_Communicators; vector <AWorker> m_Workers; // communicators and workers.
		AProgtestSolver m_Solver; // general solver.
		Buffer m_RBuffer, m_PBuffer; // general receiving and processing buffers.
		AMutex m_RMutex, m_PMutex, m_SMutex, m_SCMutex; // general receiving and processing buffers mutexes, general solver mutex, stopped companies mutex.
		ACondVar m_WCondVar; // general worker condition variable.
		size_t m_StartedCompanies, m_StoppedCompanies; // count of companies at start and count of companies that stopped working.

};

// TODO: COptimizer implementation goes here
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef __PROGTEST__
int main ( )
{
  COptimizer optimizer;
  ACompanyTest company1 = std::make_shared<CCompanyTest> ();

  optimizer . addCompany ( company1 );
  optimizer . start ( 3);
  optimizer . stop  ();
  if ( ! company1 -> allProcessed () ) throw std::logic_error ( "(some) problems were not correctly processsed" );

  return 0;  
}
#endif /* __PROGTEST__ */ 

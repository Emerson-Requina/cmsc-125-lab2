# MLFQ Configuration Justification

## 1. Introduction
The Multi-Level Feedback Queue (MLFQ) is a dynamic scheduling discipline designed to balance two competing objectives in operating systems: minimizing **response time** for interactive tasks and maximizing **throughput** for batch processes. The following configuration for `mlfq.conf` was developed based on the rules established by Corbato et al. (1962) and further refined in modern systems research (Arpaci-Dusseau & Arpaci-Dusseau, 2018).

## 2. Queue Configuration & Time Quantums

### Q0: Interactivity and Responsiveness
* **Time Quantum:** 30ms
* **Allotment:** 50ms
* **Justification:** The highest priority queue ($Q_0$) is designed for short-duration, I/O-bound tasks. According to Arpaci-Dusseau and Arpaci-Dusseau (2018), interactive jobs frequently relinquish the CPU to wait for user input. By providing a short quantum of 30ms, we ensure that the scheduler can quickly rotate through interactive tasks, maintaining a high level of system responsiveness. The 50ms allotment ensures that a process that performs multiple quick bursts (common in UI updates) remains at this priority level without being prematurely demoted to a lower queue.

### Q1: Balanced Workloads
* **Time Quantum:** 100ms
* **Allotment:** 100ms
* **Justification:** Jobs that exceed the $Q_0$ allotment are likely "medium-sized" tasks that require more computational power but are not yet identified as pure batch processes. Stallings (2018) suggests that increasing the time quantum as priority decreases allows CPU-bound processes to make meaningful progress without the overhead of frequent context switching. A 100ms quantum provides a "fair share" for these mixed-workload tasks.

### Q2: Batch Processing
* **Time Quantum:** -1 (FCFS)
* **Allotment:** -1
* **Justification:** The lowest priority queue ($Q_2$) handles long-running, CPU-intensive batch jobs. Silberschatz et al. (2018) note that for very long processes, the overhead of Round Robin (RR) scheduling provides diminishing returns. By using First-Come-First-Served (FCFS) with no specific time limit, we allow these background tasks to run until completion or until a higher-priority interactive job arrives, thus maximizing CPU utilization and throughput.



## 3. Priority Boost & Starvation Prevention

### Boost Period (S): 200 units
One of the primary risks of a feedback-based system is **starvation**, where a constant stream of high-priority interactive tasks prevents low-priority batch jobs from ever receiving CPU time (GeeksforGeeks, 2024). 

* **Justification:** To mitigate this, we implemented a **Priority Boost** every 200 time units. This period resets all processes in the system to $Q_0$. This ensures that even the longest-running background task is guaranteed a slice of the CPU periodically, adhering to the "Rule of Fairness" described in the *Operating Systems: Three Easy Pieces* framework (Arpaci-Dusseau & Arpaci-Dusseau, 2018).



## 4. References

* **Arpaci-Dusseau, R. H., & Arpaci-Dusseau, A. C. (2018).** *Operating Systems: Three Easy Pieces*. Arpaci-Dusseau Books, LLC. [https://pages.cs.wisc.edu/~remzi/OSTEP/](https://pages.cs.wisc.edu/~remzi/OSTEP/)
* **GeeksforGeeks. (2024, January 5).** *Multilevel Feedback Queue Scheduling (MLFQ) CPU Scheduling*. [https://www.geeksforgeeks.org/multilevel-feedback-queue-scheduling-mlfq-cpu-scheduling/](https://www.geeksforgeeks.org/multilevel-feedback-queue-scheduling-mlfq-cpu-scheduling/)
* **Silberschatz, A., Galvin, P. B., & Gagne, G. (2018).** *Operating System Concepts* (10th ed.). Wiley.
* **Stallings, W. (2018).** *Operating Systems: Internals and Design Principles* (9th ed.). Pearson.
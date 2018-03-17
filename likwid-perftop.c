#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <signal.h>

#include <ncurses.h>
#include <time.h>

#include <likwid.h>

static int cur_term = 0;
static int num_cpus = 0;
static int* cpus = NULL;

void handle_winch(int sig)
{
    endwin();
    refresh();
    clear();
    refresh();
}

void quit()
{
    perfmon_finalize();
    topology_finalize();
    free(cpus);
    cpus = NULL;
    num_cpus = 0;
    endwin();
}



int main(int argc, char* argv[])
{
    int ret = 0;
    char group[] = "L3";
    int groupid = -1;
    int num_cpus = 0;
    int* cpus = NULL;


    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = handle_winch;
    sigaction(SIGWINCH, &sa, NULL);

    topology_init();
    CpuTopology_t topo = get_cpuTopology();
    cpus = malloc(topo->activeHWThreads * sizeof(int));
    if (!cpus)
        return -ENOMEM;
    for (int i = 0; i <  topo->numHWThreads; i++)
    {
        if (topo->threadPool[i].inCpuSet)
        {
            cpus[num_cpus] = topo->threadPool[i].apicId;
            num_cpus++;
        }
    }

    perfmon_init(num_cpus, cpus);

    groupid = perfmon_addEventSet(group);
    initscr();
    atexit(quit);
    curs_set(0);
    //(void)echo();
    clear();
    if (groupid >= 0)
    {
        ret = perfmon_setupCounters(groupid);
        ret = perfmon_startCounters();
        sleep(1);
        for (int i = 0; i < 100; i++)
        {
            ret = perfmon_readCounters();
            clear();
            printw("Eventset: %s\n", perfmon_getGroupName(groupid));
            printw("CPU\t%15s\t%15s\t%15s\n", perfmon_getMetricName(groupid, 3), perfmon_getMetricName(groupid, 4), perfmon_getMetricName(groupid, 6));
            for (int j = 0; j < num_cpus; j++)
            {
                printw("%3d\t%10.5f\t\t\t%10.5f\t\t\t%10.5f\n", cpus[j], perfmon_getLastMetric(groupid, 3, j), perfmon_getLastMetric(groupid, 4, j), perfmon_getLastMetric(groupid, 6, j));
            }
            refresh();
            sleep(1);
        }
        ret = perfmon_stopCounters();
    }

    //perfmon_finalize();
    //topology_finalize();

    //endwin();
    //free(cpus);
    return 0;

}

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

struct sched_queue
{
    struct proc * elems[NPROC];
    int start;
    int end;
    int size;
};

static void new_queue(struct sched_queue * queue)
{
    queue->start = 0;
    queue->end = -1;
    queue->size = 0;
    queue->elems[queue->start] = 0;
}

static void queue_push(struct sched_queue * queue, struct proc *elem)
{
    if (queue->size == NPROC)
    {
        panic("Scheduler Queue size excedido.");
    }
    queue->end = (queue->end + 1) % NPROC;
    queue->elems[queue->end] = elem;
    queue->size += 1;
}

static struct proc *queue_pop(struct sched_queue * queue)
{
    if (queue->size == 0)
    {
        panic("Scheduler Queue is empty.");
    }
    struct proc * elem = queue->elems[queue->start];
    queue->start = (queue->start + 1) % NPROC;
    queue->size -= 1;
    return elem;
}

static int queue_is_empty(struct sched_queue * queue)
{
    return queue->size == 0;
}

// static struct proc * queue_first(struct sched_queue queue)
// {
//     return queue.elems[queue.start];
// }

// static struct proc * queue_last(struct sched_queue queue)
// {
//     return queue.elems[queue.end];
// }

struct spinlock queues_lock;
struct sched_queue queue_2;
struct sched_queue queue_1;
struct sched_queue queue_0;

void scheduler_init(void)
{
    initlock(&queues_lock, "Sched Queues");
    new_queue(&queue_2);
    new_queue(&queue_1);
    new_queue(&queue_0);
}

static struct sched_queue * queue_priority(int priority) {
    if (priority == 2) {
        return &queue_2;
    }
    if (priority == 1) {
        return &queue_1;
    }
    if (priority == 0) {
        return &queue_0;
    }
    panic("error");
}

// Requiere que se tenga el lock del proceso proc.
void schedule_process(struct proc * proc) {
    proc->state = RUNNABLE;
    acquire(&queues_lock);
    struct sched_queue * queue = queue_priority(proc->prio);
    queue_push(queue, proc);
    release(&queues_lock);
}

static void run_process(struct proc *p, struct cpu *c)
{
    if (p->state == RUNNABLE)
    {
        // Switch to chosen process.  It is the process's job
        // to release its lock and then reacquire it
        // before jumping back to us.
        p->state = RUNNING;
        p->selco += 1;
        c->proc = p;
        swtch(&c->context, &p->context);

        // Process is done running for now.
        // It should have changed its p->state before coming back.
        c->proc = 0;
    } else {
        panic("error");
    }
}


// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run.
//  - swtch to start running that process.
//  - eventually that process transfers control
//    via swtch back to the scheduler.
void scheduler(void)
{
    struct proc *p;
    struct cpu *c = mycpu();

    c->proc = 0;

    for (;;)
    {
        // Avoid deadlock by ensuring that devices can interrupt.
        intr_on();

        acquire(&queues_lock);
        if (!queue_is_empty(&queue_2))
        {
            p = queue_pop(&queue_2);
            release(&queues_lock);

            acquire(&p->lock);
            run_process(p, c);
            release(&p->lock);

            continue;
        }
        if (!queue_is_empty(&queue_1))
        {
            p = queue_pop(&queue_1);
            release(&queues_lock);

            acquire(&p->lock);
            run_process(p, c);
            release(&p->lock);

            continue;
        }
        if (!queue_is_empty(&queue_0))
        {
            p = queue_pop(&queue_0);
            release(&queues_lock);
            
            acquire(&p->lock);
            run_process(p, c);
            release(&p->lock);

            continue;
        }
        
        release(&queues_lock);

        // for (p = proc; p < &proc[NPROC]; p++)
        // {
        //     acquire(&p->lock);
        //     if (p->state == RUNNABLE)
        //     {
        //         // Switch to chosen process.  It is the process's job
        //         // to release its lock and then reacquire it
        //         // before jumping back to us.
        //         p->state = RUNNING;
        //         p->selco += 1;
        //         c->proc = p;
        //         swtch(&c->context, &p->context);

        //         // Process is done running for now.
        //         // It should have changed its p->state before coming back.
        //         c->proc = 0;
        //     }
        //     release(&p->lock);
        // }
    }
}
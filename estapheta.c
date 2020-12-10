#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

struct msgbuf {
    long type;
    char mtext[1];
};

void judge(int idmsg, int N)
{
    struct msgbuf buf;
    struct msgbuf st;
    st.type = N + 1;
    struct msgbuf end;
    end.type = 2 * N + 2;
    for (int i = 1; i < N + 1; i++)
    {
        msgrcv(idmsg, &buf, 1, i, 0);
        printf("judge:    Recieved message abour readiness from runner %d.\n", i);
        sync();
    }
    printf("judje:    All runners ready to start.\n");
    sync();
    msgsnd(idmsg, &st, 1, 0);
    printf("judje:    Gave stick to runner 1.\n");
    sync();
    msgrcv(idmsg, &buf, 1, 2 * N + 1, 0);
    printf("judge:    Got stick from last runner %d.\n", N);
    sync();
    printf("judge:    Competition ended. Thank for participating! You can go home.\n");
    sync();
    for (int i = 0; i < N; i++)
    {
        msgsnd(idmsg, &end, 1, 0);
    }
}

void runners(int i, int idmsg, int N)
{
    struct msgbuf msg;//message about reading
    struct msgbuf buf;
    struct msgbuf st;//stick
    st.type = N + i + 1;
    msg.type = i;
    msgsnd(idmsg, &msg, 1, 0);
    printf("runner %d: Ready to estaphet.\n", i);
    sync();
    msgrcv(idmsg, &buf, 1, N + i, 0);
    printf("runner %d: I got stick.\n", i);
    sync();
    msgsnd(idmsg, &st, 1, 0);
    if (i != N)
    {
        printf("runner %d: Gave stick to runner %d\n", i, i + 1);
        sync();
    }
    else
    {
        printf("runner %d: Gave stick to judge\n", i);
        sync();
    }
    msgrcv(idmsg, &buf, 1, 2 * N + 2, 0);
    printf("runner %d: Thank to all for having a good time! Bye!\n", i);
    sync();
}

int main(int argc, char *argv[])
{
    int n = 0;
    int idmsg = 0;
    int N = atoi(argv[1]);
    pid_t pid;
    if ((idmsg = msgget(IPC_PRIVATE, IPC_CREAT | 0600)) == -1)
    {
        perror("msgget");
        exit(1);
    }
    printf("creator:  Created message queue with id %d.\n", idmsg);
	pid = fork();
	if (pid == 0)
	{
		judge(idmsg, N);
		return 0;
	}
    for (int i = 1; i < N + 1; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            runners(i, idmsg, N);
            return 0;
        }
    }
    for (int i = 0; i < N + 1; i++)
    {
        wait(&n);
    }
    msgctl(idmsg, IPC_RMID, 0);
    printf("creator:  Removed message queue with id %d.\n", idmsg);
    return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>


int main(int argc, char *argv[])
{	
	struct passwd *p;
	struct group *g;
	int numofgroup = 10;
	gid_t list[20];
	if (argc == 2)
	{
		p = getpwnam(argv[1]);
		g = getgrnam(argv[1]);
		getgrouplist(argv[1], p->pw_gid, list, &numofgroup);
	}
	else 
	{
		p = getpwuid(getuid());
		g = getgrgid(getgid());
		numofgroup = getgroups(100, list);
	}
	printf("uid=%d(%s) gid=%d(%s) groups=%d(%s)", p->pw_uid, p->pw_name, p->pw_gid, p->pw_name, g->gr_gid, g->gr_name);
	for (int i = 0; i < numofgroup; i++)
	{
		if (list[i] != p->pw_gid)
		{
			printf(",%d(%s)", list[i], getgrgid(list[i])->gr_name);
		}
	}	
	return 0;	
}



/*
 *  linux/kernel/sys.c
 *
 *  (C) 1991  Linus Torvalds
 */

#include <errno.h>

#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <sys/times.h>
#include <sys/utsname.h>
#include <asm/io.h>

int sys_ftime()
{
	return -ENOSYS;
}

int sys_break()
{
	return -ENOSYS;
}

int sys_ptrace()
{
	return -ENOSYS;
}

int sys_stty()
{
	return -ENOSYS;
}

int sys_gtty()
{
	return -ENOSYS;
}

int sys_rename()
{
	return -ENOSYS;
}

int sys_prof()
{
	return -ENOSYS;
}

int sys_setregid(int rgid, int egid)
{
	if (rgid>0) {
		if ((current->gid == rgid) || 
		    suser())
			current->gid = rgid;
		else
			return(-EPERM);
	}
	if (egid>0) {
		if ((current->gid == egid) ||
		    (current->egid == egid) ||
		    suser()) {
			current->egid = egid;
			current->sgid = egid;
		} else
			return(-EPERM);
	}
	return 0;
}

int sys_setgid(int gid)
{
/*	return(sys_setregid(gid, gid)); */
	if (suser())
		current->gid = current->egid = current->sgid = gid;
	else if ((gid == current->gid) || (gid == current->sgid))
		current->egid = gid;
	else
		return -EPERM;
	return 0;
}

int sys_acct()
{
	return -ENOSYS;
}

int sys_phys()
{
	return -ENOSYS;
}

int sys_lock()
{
	return -ENOSYS;
}

int sys_mpx()
{
	return -ENOSYS;
}

int sys_ulimit()
{
	return -ENOSYS;
}

int sys_time(long * tloc)
{
	int i;

	i = CURRENT_TIME;
	if (tloc) {
		verify_area(tloc,4);
		put_fs_long(i,(unsigned long *)tloc);
	}
	return i;
}

/*
 * Unprivileged users may change the real user id to the effective uid
 * or vice versa.
 */
int sys_setreuid(int ruid, int euid)
{
	int old_ruid = current->uid;
	
	if (ruid>0) {
		if ((current->euid==ruid) ||
                    (old_ruid == ruid) ||
		    suser())
			current->uid = ruid;
		else
			return(-EPERM);
	}
	if (euid>0) {
		if ((old_ruid == euid) ||
                    (current->euid == euid) ||
		    suser()) {
			current->euid = euid;
			current->suid = euid;
		} else {
			current->uid = old_ruid;
			return(-EPERM);
		}
	}
	return 0;
}

int sys_setuid(int uid)
{
/*	return(sys_setreuid(uid, uid)); */
	if (suser())
		current->uid = current->euid = current->suid = uid;
	else if ((uid == current->uid) || (uid == current->suid))
		current->euid = uid;
	else
		return -EPERM;
	return(0);
}

int sys_stime(long * tptr)
{
	if (!suser())
		return -EPERM;
	startup_time = get_fs_long((unsigned long *)tptr) - jiffies/HZ;
	return 0;
}

int sys_times(struct tms * tbuf)
{
	if (tbuf) {
		verify_area(tbuf,sizeof *tbuf);
		put_fs_long(current->utime,(unsigned long *)&tbuf->tms_utime);
		put_fs_long(current->stime,(unsigned long *)&tbuf->tms_stime);
		put_fs_long(current->cutime,(unsigned long *)&tbuf->tms_cutime);
		put_fs_long(current->cstime,(unsigned long *)&tbuf->tms_cstime);
	}
	return jiffies;
}

int sys_brk(unsigned long end_data_seg)
{
	if (end_data_seg >= current->end_code &&
	    end_data_seg < current->start_stack - 16384)
		current->brk = end_data_seg;
	return current->brk;
}

/*
 * This needs some heave checking ...
 * I just haven't get the stomach for it. I also don't fully
 * understand sessions/pgrp etc. Let somebody who does explain it.
 */
int sys_setpgid(int pid, int pgid)
{
	int i;

	if (!pid)
		pid = current->pid;
	if (!pgid)
		pgid = current->pid;
	for (i=0 ; i<NR_TASKS ; i++)
		if (task[i] && task[i]->pid==pid) {
			if (task[i]->leader)
				return -EPERM;
			if (task[i]->session != current->session)
				return -EPERM;
			task[i]->pgrp = pgid;
			return 0;
		}
	return -ESRCH;
}

int sys_getpgrp(void)
{
	return current->pgrp;
}

int sys_setsid(void)
{
	if (current->leader && !suser())
		return -EPERM;
	current->leader = 1;
	current->session = current->pgrp = current->pid;
	current->tty = -1;
	return current->pgrp;
}

int sys_getgroups()
{
	return -ENOSYS;
}

int sys_setgroups()
{
	return -ENOSYS;
}

int sys_uname(struct utsname * name)
{
	static struct utsname thisname = {
		"linux .0","nodename","release ","version ","machine "
	};
	int i;

	if (!name) return -ERROR;
	verify_area(name,sizeof *name);
	for(i=0;i<sizeof *name;i++)
		put_fs_byte(((char *) &thisname)[i],i+(char *) name);
	return 0;
}

int sys_sethostname()
{
	return -ENOSYS;
}

int sys_getrlimit()
{
	return -ENOSYS;
}

int sys_setrlimit()
{
	return -ENOSYS;
}

int sys_getrusage()
{
	return -ENOSYS;
}

int sys_gettimeofday()
{
	return -ENOSYS;
}

int sys_settimeofday()
{
	return -ENOSYS;
}


int sys_umask(int mask)
{
	int old = current->umask;

	current->umask = mask & 0777;
	return (old);
}

#define vga_graph_memstart 0xA0000;
#define vga_graph_memsize 64000
#define cursor_side 6
#define vga_width 320
#define vga_height 200
int sys_init_graphics()
{
	outb(0x05, 0x3CE);
	outb(0x40, 0x3CF); //设定256色，且取出方式为移动拼装
	outb(0x06, 0x3CE);
	outb(0x05, 0x3CF); //设定显存的地址区域，禁止字符模式
	outb(0x04, 0x3C4);
	outb(0x08, 0x3C5); //设定将四个显存片连在一起

	outb(0x01, 0x3D4);
	outb(0x4F, 0x3D5); //设置 End Horizontal Display 为79
	outb(0x03, 0x3D4);
	outb(0x82, 0x3D5); //设置 Display Enable Skew 为0

	outb(0x07, 0x3D4);
	outb(0x1F, 0x3D5); // 设置 Vertical Display End 第8，9位为1，0
	outb(0x12, 0x3D4);
	outb(0x8F, 0x3D5); // 设置 Vertical Display End 低7位为0x8F
	outb(0x17, 0x3D4);
	outb(0xA3, 0x3D5); // 设置 SLDIV = 1, 将 Scanline clock 除以2

	outb(0x14, 0x3D4);
	outb(0x40, 0x3D5); //设置 DW = 1
	outb(0x13, 0x3D4);
	outb(0x28, 0x3D5); //设置 Offset = 40

	outb(0x0C, 0x3D4);
	outb(0x0, 0x3D5);
	outb(0x0D, 0x3D4);
	outb(0x0, 0x3D5); //将 Start Address 设置为 0xA0000
	//在图形模式下绘制背景
	char *p = vga_graph_memstart;
	int i;
 	for( i = 0; i < vga_graph_memsize; i++)
 		*p++ = 3; //将背景颜色设置为蓝绿色
    return 0;
}
struct object{
	long x_pos;
	long y_pos;
	long width;
	long height;
	long color;
};




int sys_paint(struct object *o)
{
int i,j;
char *p = vga_graph_memstart;
for(i = o->x_pos - cursor_side; i <= o->x_pos + cursor_side; i++)
for(j = o->y_pos - cursor_side; j <= o->y_pos + cursor_side; j++)
 {
 p = (char *)vga_graph_memstart + j * vga_width + i;
*p = 12; //鼠标颜色为红色
 }
}

#include <message.h>

int sys_timer_creater(){
	return 0;
}
struct message msg_que[1024];
unsigned int msg_que_fron = NULL, msg_que_rear = NULL;
void post_message(int type){
    if (msg_que_rear != msg_que_fron - 1) {
		struct message msg;
		msg.mid = type;
		msg.pid = current->pid;
		msg_que[msg_que_rear] = msg;
		msg_que_rear = (msg_que_rear + 1) % 1024;
	}
}
void sys_get_message(struct message *msg) {
    struct message tmp;
	if(msg_que_rear == msg_que_fron){
		put_fs_long(-1,&msg->mid);
		put_fs_long(-1,&msg->pid);
		return;
	}
	
	tmp = msg_que[msg_que_fron];
	msg_que[msg_que_fron].mid = 0;
	msg_que_fron = (msg_que_fron + 1) % 1024;;
	put_fs_long(tmp.mid,&msg->mid);
	put_fs_long(current->pid,&msg->pid);
}


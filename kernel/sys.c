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
#include <signal.h>

#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

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


struct linux_dirent {
	long           d_ino;
	off_t          d_off;
	unsigned short d_reclen;
	char           d_name[14];
};

int sys_getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count)
{
 	//定义
	struct m_inode *ino;
	struct buffer_head *head;
	struct dir_entry *dir;
	struct linux_dirent dirent;
	char *buf;
	int i,j;
	int ret = 0;
	int len_dir = sizeof(struct dir_entry);
	int len_dirent = sizeof(struct linux_dirent);
	//default
	if (!count) return -1;
	//获取inode和buffer_head
	ino = current->filp[fd]->f_inode;
	head = bread(ino->i_dev, ino->i_zone[0]);
	//遍历页目录项
	for (i = 0; i < ino->i_size; i += len_dir)
	{
		if (ret + len_dirent >= count) return 0;
		dir = (struct dir_entry *)(head->b_data + i);
		if (dir->inode)
		{
			dirent.d_ino = dir->inode;
			dirent.d_off = 0;
			dirent.d_reclen = sizeof(dirent);
			strcpy(dirent.d_name,dir->name);
			buf = &dirent;
			for (j = 0; j < dirent.d_reclen; j++)
				put_fs_byte(*(buf + j), ((char *)dirp) + j + ret);
			ret += dirent.d_reclen;
		}
		else
			continue;
	}
	brelse(head);
	return ret; 
}


int sys_pipe2()
{
	return -1;
}

int sys_sleep(unsigned int seconds)
{
	int ret;
	sys_signal(SIGALRM,1,NULL);
	if((ret = sys_alarm(seconds) )!= 0)
		return -1;
	sys_pause();
	return ret;
}


long sys_getcwd(char * buf, size_t size)
{
	//定义
	struct m_inode *m_inode,*n_inode;
	struct buffer_head * head;
	struct dir_entry *m_dir,*n_dir;
	char res[512],b[512];
	unsigned short tmpnode,rootnode;
    int nowblock,nowi_dev;
	int er,i;

	//初始化一些变量
	res[0]=0;
    m_inode=current->pwd, n_inode=current->root;
    nowblock=m_inode->i_zone[0];
	nowi_dev=m_inode->i_dev;

	//defualt
	if(!(nowblock = m_inode->i_zone[0]) 
	|| !(head = bread(m_inode->i_dev,nowblock)) 
	|| !nowi_dev)
		return NULL;		
	
	//获取节点信息
	m_dir = (struct dir_entry *) head->b_data;
	tmpnode=m_dir->inode;
	m_dir++;
	rootnode=m_dir->inode;

	//找当前目录
	while (m_inode!=n_inode)
	{
		m_inode=iget(nowi_dev,rootnode); 
		if (!(nowblock = m_inode->i_zone[0]))
			return 0;
		nowi_dev=m_inode->i_dev;		 
		if(!nowi_dev)
			return 0;
		if (!(head = bread(m_inode->i_dev,nowblock)))
			return 0;
		m_dir = (struct dir_entry *) head->b_data;
		n_dir=m_dir;
		er = m_inode->i_size / (sizeof (struct dir_entry));
		for(i=0;i<er;i++)
		{
			if(n_dir->inode==tmpnode)
			{
				strcpy(res,b);
				b[0]=0;
				strcat(b,"/");
				strcat(b,n_dir->name);
				strcat(b,res);
				break;
			}
			n_dir++;
		}
		tmpnode=rootnode;
		rootnode=(m_dir++)->inode;
	}
	//取出当前工作目录并返回
	size_t len=strlen(res);
	if(len==0)
	{
		strcpy(res,"/");
		len++;
	}
	//default
	if(size<len)
	{
		return 0;
	}
	char *k=buf;
	for(i=0;i<len;++i)
		put_fs_byte(res[i],k++);
	return buf;
}

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
}

	//绘制鼠标
	#define vga_graph_memstart 0xA0000;
	#define vga_graph_memsize 64000
	#define cursor_side 6
	#define vga_width 320
	#define vga_height 200
	char *p = vga_graph_memstart;
	for(int i = 0; i < vga_graph_memsize; i++)
		*p++ = 3; //将背景颜色设置为蓝绿色
	for(int i = x_pos - cursor_side; i <= x_pos + cursor_side; i++)
		for(int j = y_pos - cursor_side; j <= y_pos + cursor_side; j++)
			{
			p = (char *)vga_graph_memstart + j * vga_width + i;
			*p = 12; //鼠标颜色为红色
			}
#ifndef _MESSAGE_H
#define _MESSAGE_H
struct message{
    int mid; //消息的ID
    int pid; //消息的目标进程ID，如果是当前进程，可设为-1

}message; 
extern struct message msg_que[1024];
extern void post_message(int type);
extern void sys_get_message(struct message *msg);
#define MSG_MOUSE_CLICK 1
#endif
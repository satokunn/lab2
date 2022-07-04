#ifndef _MESSAGE_H
#define _MESSAGE_H
struct message{
    int mid; //消息的ID
    int pid; //消息的目标进程ID，如果是当前进程，可设为-1

}message; 
extern struct message msg_que[1024];
extern void post_message(int type);
extern void sys_get_message(struct message *msg);
// mid取值
#define MSG_MOUSE_LEFT_DOWN 1
#define MSG_MOUSE_RIGHT_DOWN 2
#define MSG_MOUSE_CENTER_DOWN 3
#define MSG_USER_TIMER 4
#define TYPE_USER_TIMER_INFTY 5
#define TYPE_USER_TIMER_ONCE 6

#endif
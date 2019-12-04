
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/socket.h>
#include <linux/slab.h>
#include <linux/in.h>
#include <linux/net.h>
#include <linux/syscalls.h>
#include <asm/uaccess.h>
#include <net/sock.h>

init_module(void)
{
  struct socket *sockt;
  struct sockaddr_in *server;
  struct iovec *iov;
  struct msghdr *sock_msg;
  int error,suc;
  char *buff;
  mm_segment_t old_mm;
  int size;
  
   printk("DEBUG: start\n");
  server=(struct sockaddr_in*) kmalloc(sizeof(struct sockaddr_in),GFP_KERNEL);
  iov = (struct iovec*)kmalloc(sizeof(struct iovec),GFP_KERNEL);
  sock_msg = (struct msghdr*)kmalloc(sizeof(struct msghdr),GFP_KERNEL);
  buff = (char*) kmalloc(((sizeof(char))*200), GFP_KERNEL);
  // «Ø¥ß€@ socket
  error = sock_create(AF_INET, SOCK_STREAM, 0, &sockt);
  if(error<0) { // ­Y sock_create €£Šš¥\¡A«h°h¥X
    printk(KERN_ALERT "socket failed %d\n", error);
    return 0;
  }
  printk(KERN_ALERT "client sock %d\n", error);
  memset(server,0,sizeof(struct sockaddr_in));
  server->sin_family = AF_INET;
  server->sin_addr.s_addr = in_aton("127.0.0.1");
  server->sin_port = htons(9734);        // ³sœu¥Øªºport¬° 4321
  // °ÝÃD±ÀŽú¬O¥XŠb³ožÌ¡AµLªk connect ŠÜ¥Øªº¥DŸ÷

  error=sockt->ops->connect(sockt,(struct sockaddr*)server,sizeof(server), !O_NONBLOCK);
 printk(KERN_ALERT "connect: %d\n", error);
  if (error<0) return 0; // ­Yconnect¥¢±Ñ¡A«h°h¥X
 
  
  
  old_mm = get_fs();
  set_fs(KERNEL_DS);
  memset(iov,0,sizeof(struct iovec));
  memset(sock_msg,0,sizeof(struct msghdr));
  buff[0] = 'a';  // ³]©w±ý¶Ç°eªºžê®Æšì buff
  iov[0].iov_base = (void*)buff;
  size = 100;
  iov[0].iov_len = size;
  sock_msg->msg_name = NULL;
  sock_msg->msg_namelen = 0;
  sock_msg->msg_iov = iov;
  sock_msg->msg_iovlen = 1;
  sock_msg->msg_control = NULL;
  sock_msg->msg_controllen = 0;
  sock_msg->msg_flags = 0;
  error = sock_sendmsg(sockt, sock_msg, size); //¶Ç°ežê®ÆŠÜ¥Øªº¥DŸ÷
  printk(KERN_ALERT "send msg in client success:%d\n", error);
  set_fs(old_mm);
  sock_release(sockt);
}
void cleanup_module(void)
{
	
	printk(KERN_INFO "Goodbye client\n");
}

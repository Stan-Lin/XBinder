#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/socket.h>
#include <linux/net.h>
#include <net/sock.h>
#define port 9734

  struct socket*        sock = NULL;
int  init_module(void)
{

  struct sockaddr_in*   dest = {0};
  int                   retVal = 0;

  int size,error;
  struct msghdr *sock_msg;
  char *buff;
  struct iovec *iov;
	
 

  dest = (struct sockaddr_in*)kmalloc(sizeof(struct sockaddr_in), GFP_KERNEL);
  sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
  dest->sin_family = AF_INET;
  dest->sin_addr.s_addr = in_aton("140.116.72.251");
  dest->sin_port = htons(port);
  printk(KERN_EMERG "Connect to %X:%u\n", dest->sin_addr.s_addr, port);

  retVal = sock->ops->connect(sock, (struct sockaddr*)dest, sizeof(struct sockaddr_in), !O_NONBLOCK);

	
  if (retVal >= 0) {
    printk(KERN_EMERG "Connected\n");

 
  }
  else
    printk(KERN_EMERG "Error %d\n", -retVal); 

  return (0);
}
void cleanup_module(void)
{
	sock_release(sock);
	printk(KERN_INFO "Goodbye server\n");
}

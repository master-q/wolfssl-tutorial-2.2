#include	"unp.h"
#include	<cyassl/ssl.h>

void sig_handler(const int sig) 
{
    printf("\nSIGINT handled.\n");
	CyaSSL_Cleanup();			/* Free CyaSSL */
	
    /* We can't free the CYASSL_CTX here because the 'ctx' variable is 
       out of scope.  As such, we let the OS free this resource when the 
       program terminates. */

    exit(EXIT_SUCCESS);
}

void
str_cli(FILE *fp, CYASSL* ssl)
{
	char	sendline[MAXLINE], recvline[MAXLINE];
	int		n = 0;

	while (Fgets(sendline, MAXLINE, fp) != NULL) {

		if(CyaSSL_write(ssl, sendline, strlen(sendline)) != 
                strlen(sendline)){
			err_sys("CyaSSL_write failed");
		}

		if ((n = CyaSSL_read(ssl, recvline, MAXLINE)) <= 0)
			err_quit("CyaSSL_read error");

		recvline[n] = '\0';
		Fputs(recvline, stdout);
	}
}

int
main(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_in	servaddr;
	
	/* define a signal handler for when the user closes the program 
       with Ctrl-C */
	signal(SIGINT, sig_handler);

	if (argc != 2)
		err_quit("usage: tcpcli <IPaddress>");
		
	CyaSSL_Init();	// Initialize CyaSSL
	CYASSL_CTX* ctx;
	
	/* Create and initialize CYASSL_CTX structure */
	if ( (ctx = CyaSSL_CTX_new(CyaTLSv1_2_client_method())) == NULL){
		fprintf(stderr, "SSL_CTX_new error.\n");
		exit(EXIT_FAILURE);
	}
	
	/* Load CA certificates into CYASSL_CTX.
	   These will be used to verify the server we connect to */
	if (CyaSSL_CTX_load_verify_locations(ctx,"../certs/ca-cert.pem",0) != 
            SSL_SUCCESS) {
		fprintf(stderr, "Error loading ../certs/ca-cert.pem, "
                "please check the file.\n");
		exit(EXIT_FAILURE);
	}

	/* Create Socket file descriptor */
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	/* Connect to socket file descriptor */
	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
	
	/* Create CYASSL object */
	CYASSL* ssl;
	
	if( (ssl = CyaSSL_new(ctx)) == NULL) {
		fprintf(stderr, "CyaSSL_new error.\n");
		exit(EXIT_FAILURE);
	}
	
	CyaSSL_set_fd(ssl, sockfd);

	str_cli(stdin, ssl);		/* do it all */
	
	CyaSSL_free(ssl);		// Free SSL object
	CyaSSL_CTX_free(ctx);	// Free SSL_CTX object
	CyaSSL_Cleanup();		// Free CyaSSL
	exit(0);
}

#include	"unp.h"
#include	<cyassl/ssl.h>

static int cleanup;		// To handle shutdown

void sig_handler(const int sig) 
{
    printf("\nSIGINT handled.\n");
	cleanup = 1;
	return;
}

void
str_echo(CYASSL* ssl)
{
	int			n;
	char		buf[MAXLINE];


	while ( (n = CyaSSL_read(ssl, buf, MAXLINE)) > 0) {
		if(CyaSSL_write(ssl, buf, n) != n) {
			err_sys("CyaSSL_write failed");
		}
	}

	if( n < 0 )
		printf("CyaSSL_read error = %d\n", CyaSSL_get_error(ssl,n));
		
	else if( n == 0 )
		printf("The peer has closed the connection.\n");
}

int
main(int argc, char **argv)
{
	int					listenfd, connfd;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
	char				buff[MAXLINE];
	int					optval;			/* flag value for setsockopt */
	struct sigaction	act, oact;		/* structures for signal handling */
		
	/* Define a signal handler for when the user closes the program
       with Ctrl-C. Also, turn off SA_RESTART so that the OS doesn't 
       restart the call to accept() after the signal is handled. */
	
	act.sa_handler = sig_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGINT, &act, &oact);
	
	CyaSSL_Init();      // Initialize CyaSSL
	CYASSL_CTX* ctx;
	
	/* Create and initialize CYASSL_CTX structure */
	if ( (ctx = CyaSSL_CTX_new(CyaTLSv1_2_server_method())) == NULL){
		fprintf(stderr, "CyaSSL_CTX_new error.\n");
		exit(EXIT_FAILURE);
	}
	
	/* Load CA certificates into CYASSL_CTX */
	if (CyaSSL_CTX_load_verify_locations(ctx,"../certs/ca-cert.pem",0) != 
            SSL_SUCCESS) {
		fprintf(stderr, "Error loading ../certs/ca-cert.pem, "
                "please check the file.\n");
		exit(EXIT_FAILURE);
	}
	
	/* Load server certificate into CYASSL_CTX */
	if (CyaSSL_CTX_use_certificate_file(ctx,"../certs/server-cert.pem", 
                SSL_FILETYPE_PEM) != SSL_SUCCESS) {
	   fprintf(stderr, "Error loading ../certs/server-cert.pem, "
               "please check the file.\n");
	   exit(EXIT_FAILURE);
	}

	/* Load server key into CYASSL_CTX */
	if (CyaSSL_CTX_use_PrivateKey_file(ctx,"../certs/server-key.pem", 
                SSL_FILETYPE_PEM) != SSL_SUCCESS) {
	   fprintf(stderr, "Error loading ../certs/server-key.pem, "
               "please check the file.\n");
	   exit(EXIT_FAILURE);
	}

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	
	/* setsockopt: Eliminates "ERROR on binding: Address already in 
       use" error. */
	optval = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , 
            sizeof(int));

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));	
	Listen(listenfd, LISTENQ);

	while(cleanup != 1)
	{	
		clilen = sizeof(cliaddr);
		CYASSL* ssl;
		
		if ( (connfd = accept(listenfd, (SA *) &cliaddr, &clilen)) < 0 ) 
		{
			if (errno == EINTR)
				continue;		/* back to while() */
			else
				err_sys("accept error\n");
		}
		
		printf("Connection from %s, port %d\n", 
				inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
				ntohs(cliaddr.sin_port));
		
		/* Create CYASSL Object */
		if( (ssl = CyaSSL_new(ctx)) == NULL) {
		   fprintf(stderr, "CyaSSL_new error.\n");
		   exit(EXIT_FAILURE);
		}

		CyaSSL_set_fd(ssl, connfd);
		str_echo(ssl);				/* process the request */	
		CyaSSL_free(ssl);			/* Free CYASSL object */
		Close(connfd);				/* close connected socket */
	}
	
	CyaSSL_CTX_free(ctx);              /* Free CYASSL_CTX */
	printf("CyaSSL_CTX freed\n");
	CyaSSL_Cleanup();			       /* Free CyaSSL */
	printf("CyaSSL freed\n");
    exit(EXIT_SUCCESS);
}

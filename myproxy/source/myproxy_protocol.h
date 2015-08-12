/*
 *
 * MyProxy protocol API
 *
 */
#ifndef __MYPROXY_PROTOCOL_H
#define __MYPROXY_PROTOCOL_H

/* Protocol commands */
typedef enum
{
    MYPROXY_GET_PROXY,
    MYPROXY_PUT_PROXY,
    MYPROXY_INFO_PROXY,
    MYPROXY_DESTROY_PROXY,
    MYPROXY_CHANGE_CRED_PASSPHRASE,
    MYPROXY_STORE_CERT,
    MYPROXY_RETRIEVE_CERT,
    MYPROXY_GET_TRUSTROOTS
} myproxy_proto_request_type_t;

/* server response codes */
typedef enum
{
    MYPROXY_OK_RESPONSE,
    MYPROXY_ERROR_RESPONSE,
    MYPROXY_AUTHORIZATION_RESPONSE
} myproxy_proto_response_type_t;

/* client/server socket attributes */
typedef struct myproxy_socket_attrs_s
{
  char *pshost;	
  int psport;
  int socket_fd;
  struct _gsi_socket *gsi_socket; 
} myproxy_socket_attrs_t;

/* A client request object */
#define REGULAR_EXP 1
#define MATCH_CN_ONLY 0

typedef struct
{
    char                         *version;
    char                         *username;
    char                         passphrase[MAX_PASS_LEN+1];
    char                         new_passphrase[MAX_PASS_LEN+1];
    myproxy_proto_request_type_t command_type;
    int                          proxy_lifetime;
    char                         *retrievers;
    char                         *renewers;
    char			 *credname;
    char			 *creddesc;
    char			 *authzcreds;
    char 		         *keyretrieve;
    char                         *trusted_retrievers;
    int                          want_trusted_certs; /* 1=yes, 0=no */
    char                         *voname;
    char                         *vomses;
    char                         *certreq;
} myproxy_request_t;

/* A server response object */
typedef struct
{
  char                          *version;
  myproxy_proto_response_type_t response_type;
  authorization_data_t		**authorization_data;
  char				*error_string;
  myproxy_creds_t		*info_creds;
  myproxy_certs_t               *trusted_certs;
} myproxy_response_t;

  
/*
 * myproxy_init_client()
 *
 * Create a generic client by creating a GSI socket and connecting to a a host 
 *
 * returns the file descriptor of the connected socket or
 *   -1 if an error occurred
 */
int myproxy_init_client(myproxy_socket_attrs_t *attrs);

/*
 * myproxy_authenticate_init()
 * 
 * Perform client-side authentication
 *
 * returns -1 if unable to authenticate, 0 if authentication successful
 */ 
int myproxy_authenticate_init(myproxy_socket_attrs_t *attr,
			      const char *proxyfile);

/*
 * myproxy_authenticate_accept()
 * 
 * Perform server-side authentication and retrieve the client's DN
 *
 * returns -1 if unable to authenticate, 0 if authentication successful
 */ 
int myproxy_authenticate_accept(myproxy_socket_attrs_t *attr, 
                                char *client_name, const int namelen);

/*
 * myproxy_authenticate_accept_fqans()
 *
 * The same as myproxy_authenticate_accept() but also returns a list of FQANs
 * if suggested by the peer.
 *
 */
int myproxy_authenticate_accept_fqans(myproxy_socket_attrs_t *attr,
                                      char *client_name, const int namelen,
				      char ***fqans); 

/*
 * myproxy_serialize_request()
 * 
 * Serialize a request object into a buffer to be sent over the network.
 * Use myproxy_serialize_request_ex() instead.
 *
 * Returns the serialized data length or -1 on error.
 */
int myproxy_serialize_request(const myproxy_request_t *request, 
			      char *data, const int datalen);

/*
 * myproxy_serialize_request_ex()
 * 
 * Serialize a request object into a newly allocated buffer of correct size.
 * The caller should free() the buffer after use.
 *
 * Returns the serialized data length or -1 on error.
 */
int myproxy_serialize_request_ex(const myproxy_request_t *request, 
				 char **data);


/*
 * myproxy_deserialize_request()
 * 
 * Deserialize a buffer into a request object.
 *
 * returns 0 if succesful, otherwise -1
 */
int myproxy_deserialize_request(const char *data, const int datalen, 
				myproxy_request_t *request);

/*
 * myproxy_serialize_response()
 * 
 * Serialize a response object into a buffer to be sent over the network.
 * Use myproxy_serialize_response_ex() instead.
 *
 * returns the number of characters put into the buffer 
 * (not including the trailing NULL)
 */
int
myproxy_serialize_response(const myproxy_response_t *response, 
                           char *data, const int datalen); 

/*
 * myproxy_serialize_response_ex()
 * 
 * Serialize a response object into a newly allocated buffer of correct size.
 * The caller should free() the buffer after use.
 *
 * returns the number of characters put into the buffer 
 * (not including the trailing NULL)
 */
int
myproxy_serialize_response_ex(const myproxy_response_t *response, 
			      char **data); 

/*
 * myproxy_deserialize_response()
 *
 * Serialize a a buffer into a response object.
 *
 * returns the number of characters put into the buffer 
 * (not including the trailing NULL)
 */
int myproxy_deserialize_response(myproxy_response_t *response, 
				 const char *data, const int datalen);

/*
 * myproxy_send()
 * 
 * Sends a buffer
 *
 * returns 0 on success, -1 on error
 */
int myproxy_send(myproxy_socket_attrs_t *attrs,
                 const char *data, const int datalen);

/*
 * myproxy_recv()
 *
 * Receives a message into the buffer.
 * Use myproxy_recv_ex() instead.
 *
 * returns bytes read on success, -1 on error, -2 on truncated response
 * 
 */
int myproxy_recv(myproxy_socket_attrs_t *attrs,
		 char *data, const int datalen);

/*
 * myproxy_recv_ex()
 *
 * Receives a message into a newly allocated buffer of correct size.
 * The caller must deallocate the buffer.
 *
 * returns bytes read on success, -1 on error
 * 
 */
int myproxy_recv_ex(myproxy_socket_attrs_t *attrs, char **data);

/*
 * myproxy_init_delegation()
 *
 * Delegates a proxy based on the credentials found in file 
 * location delegfile good for lifetime_seconds
 *
 * returns 0 on success, -1 on error 
 */
int myproxy_init_delegation(myproxy_socket_attrs_t *attrs,
			    const char *delegfile,
			    const int lifetime_seconds,
			    char *passphrase);

/*
 * myproxy_accept_delegation()
 *
 * Accepts delegated credentials into a file, and sets
 * path in provided buffer.
 *
 * returns 0 on success, -1 on error 
 */
int myproxy_accept_delegation(myproxy_socket_attrs_t *attrs, char *delegfile,
			      const int delegfile_len, char *passphrase);

/*
 * myproxy_accept_delegation_ex()
 *
 * Accepts delegated credentials into a newly allocated buffer.
 * The caller must deallocate the buffer.
 * Private key is encrypted with passphrase, if provided (may be NULL).
 *
 * returns 0 on success, -1 on error 
 */
int myproxy_accept_delegation_ex(myproxy_socket_attrs_t *attrs,
				 char **credentials,
				 int *credential_len, char *passphrase);

/*
 * myproxy_request_cert()
 *
 * An alternative to myproxy_accept_delegation_ex() that takes the
 * location of a file containing a PEM-formatted certificate request
 * (certreq) as input.
 * Accepts delegated credentials into a newly allocated buffer.
 * The caller must deallocate the buffer.
 *
 * return 0 on success, -1 on error
 */
int
myproxy_request_cert(myproxy_socket_attrs_t *attrs, char *certreq,
                     char **credentials, int *credential_len);

/*
 * myproxy_accept_credentials()
 *
 * Accepts credentials into file location data
 *
 * returns 0 on success, -1 on error
 */
int
myproxy_accept_credentials(myproxy_socket_attrs_t *attrs,
                           char                   *delegfile,
                           int                     delegfile_len);

/*
 * myproxy_init_credentials()
 *
 * returns 0 on success, -1 on error 
 */
int
myproxy_init_credentials(myproxy_socket_attrs_t *attrs,
                         const char             *delegfile);

int
myproxy_get_credentials(myproxy_socket_attrs_t *attrs,
                         const char             *delegfile);

/*
 * myproxy_free()
 * 
 * Frees up memory used for creating request, response and socket objects 
 */
void myproxy_free(myproxy_socket_attrs_t *attrs, myproxy_request_t *request,
		  myproxy_response_t *response);

/*
 * myproxy_recv_response()
 *
 * Helper function that combines myproxy_recv() and
 * myproxy_deserialize_response() with some error checking.
 *
 */
int myproxy_recv_response(myproxy_socket_attrs_t *attrs,
			  myproxy_response_t *response); 

/*
 * myproxy_handle_response()
 *
 * Helper function that combines 
 * myproxy_deserialize_response() with some error checking.
 *
 */
int myproxy_handle_response(const char *response_buffer,
                            int responselen,
                            myproxy_response_t *response); 

/*
 * myproxy_recv_response_ex()
 *
 * Helper function that combines myproxy_recv(),
 * myproxy_deserialize_response(), and myproxy_handle_authorization()
 * with some error checking.
 *
 */
int myproxy_recv_response_ex(myproxy_socket_attrs_t *attrs,
			     myproxy_response_t *response,
			     myproxy_request_t *client_request);

/*
 * myproxy_handle_authorization()
 *
 * If MYPROXY_AUTHORIZATION_RESPONSE is received, pass it to this
 * function to be processed.
 *
 */
int myproxy_handle_authorization(myproxy_socket_attrs_t *attrs,
				 myproxy_response_t *server_response,
				 myproxy_request_t *client_request);

/*
 * myproxy_bootstrap_trust()
 *
 * Get server's CA certificate(s) via the SSL handshake and install
 * them in the trusted certificates directory.
 * 
 */
int myproxy_bootstrap_trust(myproxy_socket_attrs_t *attrs);

/*
 * myproxy_bootstrap_client()
 *
 * Connect to server and authenticate.
 * Bootstrap trust roots as needed/requested.
 * Allows anonymous authentication.
 * Called by myproxy-logon and myproxy-get-trustroots.
 *
 */
int myproxy_bootstrap_client(myproxy_socket_attrs_t *attrs,
                             int bootstrap_if_no_cert_dir,
                             int bootstrap_even_if_cert_dir_exists);

/*
 * myproxy_request_add_voname()
 *
 * Adds VONAME parameter to client request.
 * returns 0 if succesful, otherwise -1
 *
 */
int myproxy_request_add_voname(myproxy_request_t *client_request, 
                               const char *voname);

/*
 * myproxy_request_add_vomses()
 *
 * Adds VOMSES parameter to client request.
 * returns 0 if succesful, otherwise -1
 *
 */
int myproxy_request_add_vomses(myproxy_request_t *client_request, 
                               const char *vomses);

#endif /* __MYPROXY_PROTOCOL_H */

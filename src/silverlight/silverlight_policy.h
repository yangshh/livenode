#ifndef	__SILVERLIGHT_POLICY_H__
#define	__SILVERLIGHT_POLICY_H__
#define	SILVERLIGHT_PORT	943
#define	SILVERLIGHT_POLICY_MSG	 "<policy-file-request/>"
#define	SILVERLIGHT_POLICY_FILE	"./etc/PolicyFile.xml"
#define	SILVERLIGHT_POLICY_XML "<?xml version=\"1.0\" encoding =\"utf-8\"?><access-policy><cross-domain-access><policy><allow-from><domain uri=\"*\"/></allow-from><grant-to><socket-resource port=\"4502-4526\" protocol=\"tcp\"/></grant-to></policy></cross-domain-access></access-policy>"
#define	SPX_LEN	1024
void* silverlight_policy_process(void *arg);

#endif //__SILVERLIGHT_POLICY_H__


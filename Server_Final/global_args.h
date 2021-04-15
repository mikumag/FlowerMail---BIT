#ifndef GLOBAL_ARGS_H
#define GLOBAL_ARGS_H

/*
 * Global Args
 */

#define SERVER_PORT 8888
#define HOST "127.0.0.1"
#define USER_NAME "root"
#define PASSWD "123"
#define DATABASE "mysql"
#define PORT 3306

/*
 * LIST OF OPERATIONS
 *
 * - QUIT
 * - LOGIN
 * - REGIST
 * - SEND
 *
 * - R_ALL
 * - R_BIN
 * - R_STAR
 * - R_SENT
 * - R_DRAFT
 *
 * - DELETE
 * - BIN     // to bin
 * - SEARCH
 * - STAR    // to star
 * - DRAFT
 *
 */
#define LOGIN "LOGIN"
#define REGIST "REGIST"
#define SEND "SEND"
#define QUIT "QUIT"
#define R_ALL "R_ALL"

#define TYPE_ALL 'a'
#define TYPE_READ 'r'
#define TYPE_UNREAD 'u'


#endif // GLOBAL_ARGS_H

#include <iostream>
#include "global_args.h"
#include "global_requirements.h"
#include "Classes.h"

using namespace std;

int main()
{
    // 调用socket函数返回的文件描述符
    int serverSocket;

    // 声明两个套接字sockaddr_in结构体变量，分别表示客户端和服务器
    struct sockaddr_in server_addr; // 服务端socket
    struct sockaddr_in clientAddr;  // 客户端socket

    int addr_len = sizeof(clientAddr); // 地址长
    int client;                        // 客户端
    char userbuffer[20];               // 缓存
    char buffer[1024];                 // 缓存
    int iDataNum;                      // 缓存
    char msg[64] = "Successfully connected.\n";

    struct userInfo user;       // 用户对象
    struct mail mail;           // 邮件对象
    struct socketCtrl controll; // socket控制符对象
    struct mailList list;       // 邮件列表

    MYSQL *conn_ptr;            // MYSQL指针
    MYSQL_RES *res_ptr;         // MYSQL返回资源
    MYSQL_ROW sqlrow;           // MYSQL返回二维表的第一行

    int res;                    // 结果
    char sql[1024];             // 存放sql语句的数组
    char status[16];            // 状态
    int userid;                 // 用户状态
    char type;
    unsigned long rows;
    int cols;


    // socket linking
    if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket");
        return 1;
    }

    bzero(&server_addr, sizeof(server_addr));

    // 初始化服务器端的套接字，并用htons和htonl将端口和地址转成网络字节序
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    // ip可是是本服务器的ip，也可以用宏INADDR_ANY代替，代表0.0.0.0，表明所有地址
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 对于bind，accept之类的函数，里面套接字参数都是需要强制转换成(struct sockaddr *)
    // bind三个参数：服务器端的套接字的文件描述符，
    if(bind(serverSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("connect");
        return 1;
    }

    // 设置服务器上的socket为监听状态
    if(listen(serverSocket, 5) < 0) {
        perror("listen");
        return 1;
    }

    cout << "Done Preparaton." << endl;

    while(1)
    {
        printf("Listening on port: %d\n", SERVER_PORT);

        /*调用accept函数后，会进入阻塞状态
         * accept返回一个套接字的文件描述符，这样服务器端便有两个套接字的文件描述符
         * serverSocket和client。
         * serverSocket仍然继续在监听状态，client则负责接收和发送数据
         * clientAddr是一个传出参数，accept返回时，传出客户端的地址和端口号
         * addr_len是一个传入-传出参数，传入的是调用者提供的缓冲区的clientAddr的长度，以避免缓冲区溢出。
         * 传出的是客户端地址结构体的实际长度。
         * 出错返回-1 */

        client = accept(serverSocket, (struct sockaddr*)&clientAddr, (socklen_t*)&addr_len);
        if(client < 0){
            perror("accept");
            continue;
        }

        if (!fork()) {
            printf("\n receiving client data...\n");

            printf("Connected via IP: %s \n", inet_ntoa(clientAddr.sin_addr));
            printf("Connected via Port: %d \n", htons(clientAddr.sin_port));

            while(1) // echo
            {
                // buffer
                memset(buffer, 0, 1024);

                iDataNum = recv(client, buffer, 1024, 0);

                if(iDataNum < 0){
                    perror("recv");
                    continue;
                }

                printf("Data receoved. Length :%d, Content:  %s. \n", iDataNum, buffer);

                /*
                 * LIST OF OPERATIONS
                 * socket操作目录
                 *
                 * - QUIT 退出
                 * - LOGIN 登陆
                 * - REGIST 注册
                 * - SEND 发件

                 * - R-ALL 查看收件
                 * - R-BIN 查看回收站
                 * - R-STAR 查看收藏夹
                 * - R-SENT 查看已发送
                 * - R-DRAFT 查看草稿箱

                 * - BIN 加回收站
                 * - STAR 收藏
                 * - DRAFT 存草稿
                 * - SEARCH 搜索邮件

                 *
                 *
                 */

                /* -------------------------------------------------*
                 *                      QUIT
                 *                      退出
                 * -------------------------------------------------*/
                if(strncmp(buffer, QUIT, 4) == 0){
                    printf("Client quit \n");
                    close(client);
                    break;
                }

                /* -------------------------------------------------*
                 *                     LOGIN
                 *                     登陆
                 * -------------------------------------------------*/
                else if(strncmp(buffer, LOGIN, 4) == 0){

                    memset(&mail, 0, 1024);
                    memcpy(&mail, buffer, 1024);

                    printf("Client Login\n");


                    // SQL
                    conn_ptr = mysql_init(NULL);
                    if (!conn_ptr) {
                        printf("mysql_init failed\n");
                        return EXIT_FAILURE;
                    }

                    conn_ptr = mysql_real_connect(conn_ptr, HOST, USER_NAME, PASSWD, DATABASE, PORT, NULL, 0);


                    // Check Legality of User
                    int flag = -1;

                    if (conn_ptr) {
                        printf("\n Db connect OK\n");
                        memset(sql, 0, 1024);

                        sprintf(sql, "SELECT COUNT(*) FROM clients WHERE clients.id = %d AND clients.passwordd = '%s';", mail.user.id, mail.user.password);
                        printf("SQL: %s\n", sql);
                        res = mysql_query(conn_ptr, sql);
                        res_ptr = mysql_store_result(conn_ptr);
                        sqlrow = mysql_fetch_row(res_ptr);
                        if(sqlrow[0] == 0) {
                            flag = -1;
                        } else {
                            flag = 0;
                        } cout << "the current flag is:" << flag << endl;

                    } else {
                        printf("Connection failed\n");
                    }
                    mysql_close(conn_ptr);

                    if(flag == 0) {

                        conn_ptr = mysql_init(NULL);
                        conn_ptr = mysql_real_connect(conn_ptr, HOST, USER_NAME, PASSWD, DATABASE, PORT, NULL, 0);

                        // Query total mails

                        memset(sql, 0, 1024);
                        sprintf(sql, "SELECT COUNT(*) FROM mail WHERE receiverID = %d;", mail.user.id);
                        printf("SQL: %s\n", sql);

                        res = mysql_query(conn_ptr, sql);
                        res_ptr = mysql_store_result(conn_ptr);

                        sqlrow = mysql_fetch_row(res_ptr);
                        cout << sqlrow[0] << endl;

                        flag = atoi(sqlrow[0]);
                        mysql_close(conn_ptr);
                    }
                    cout << "Log in Success." << endl;
                    cout << "Mail Count:" << flag << endl;

                    send(client, &flag , sizeof(int), 0);
                }

                /* -------------------------------------------------*
                 *                     REGIST
                 *                     注册
                 * -------------------------------------------------*/
                else if (strncmp(buffer, REGIST, 6) == 0) {
                     memset(&mail, 0, 1024);
                     memcpy(&mail, buffer, 1024);

                     printf("Client Register\n");

                     // SQL
                     conn_ptr = mysql_init(NULL);

                     if (!conn_ptr) {
                         printf("mysql_init failed\n");
                         return EXIT_FAILURE;
                     }

                     conn_ptr = mysql_real_connect(conn_ptr, HOST, USER_NAME, PASSWD, DATABASE, PORT, NULL, 0);

                     int flag = 0;

                     if (conn_ptr) {
                         printf("\n Db connect OK\n");
                         memset(sql, 0, 1024);

                         sprintf(sql, "insert into clients values (%d,'%s')", mail.user.id, mail.user.password);
                         printf("SQL: %s\n", sql);

                         res = mysql_query(conn_ptr, sql);
                         if (!res) {     //输出受影响的行数
                             flag = 1;
                             printf("Inserted %lu rows\n",(unsigned long)mysql_affected_rows(conn_ptr));
                         }  else {       //打印出错误代码及详细信息
                             fprintf(stderr, "Insert error %d: %sn",mysql_errno(conn_ptr),mysql_error(conn_ptr));
                         }

                     } else {
                         printf("Connection failed\n");
                     }
                     mysql_close(conn_ptr);

                     cout << "Register Success. Info:" << flag << endl;
                     send(client, &flag , sizeof(int), 0);
                }

                /* -------------------------------------------------*
                 *                     SEND
                 *                     发件
                 * -------------------------------------------------*/
                else if (strncmp(buffer, SEND, 4) == 0) {
                    memset(&mail, 0, 1024);
                    memcpy(&mail, buffer, 1024);

                    printf("Client Send a Mail\n");

                    conn_ptr = mysql_init(NULL);
                    if (!conn_ptr) {
                        printf("mysql_init failed\n");
                        return EXIT_FAILURE;
                    }

                    conn_ptr = mysql_real_connect(conn_ptr, HOST, USER_NAME, PASSWD, DATABASE, PORT, NULL, 0);

                    int flag = 0;

                    if (conn_ptr) {

                        printf("\n Db connect OK\n");
                        memset(sql, 0, 1024);

                        sprintf(sql, "insert into mail (senderID, receiverID, isRecStar, isSendStar, "
                                     "isRecTrash, isSendTrash, isRecBin, isSendBin, isDraft, title, content) "
                                     "values (%d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', '%s');", mail.info.senderID, mail.info.receiverID,
                                     mail.info.isRecStar, mail.info.isRecTrash,
                                     mail.info.isRecTrash, mail.info.isSendTrash, mail.info.isRecBin, mail.info.isSendBin,
                                     mail.info.isDraft, mail.info.title, mail.content);

                        printf("SQL: %s\n", sql);

                        res = mysql_query(conn_ptr, sql);
                        if (!res) {     //输出受影响的行数
                            flag = 1;
                            printf("Inserted %lu rows\n",(unsigned long)mysql_affected_rows(conn_ptr));
                        }  else {       //打印出错误代码及详细信息
                            fprintf(stderr, "Insert error %d: %sn",mysql_errno(conn_ptr),mysql_error(conn_ptr));
                        }

                    } else {
                        printf("Connection failed\n");
                    }
                    mysql_close(conn_ptr);
                    cout << "Mail Sent! flag:" << flag << endl;
                    send(client, &flag , sizeof(int), 0);
                }

                /* -------------------------------------------------*
                 *                     R-ALL
                 *                     收件箱
                 * -------------------------------------------------*/

                else if (strncmp(buffer, R_ALL, 4) == 0) {
                    memset(&mail, 0, 1024);
                    memcpy(&mail, buffer, 1024);

                    printf("Client Send a Mail\n");

                    conn_ptr = mysql_init(NULL);
                    if (!conn_ptr) {
                        printf("mysql_init failed\n");
                        return EXIT_FAILURE;
                    }

                    conn_ptr = mysql_real_connect(conn_ptr, HOST, USER_NAME, PASSWD, DATABASE, PORT, NULL, 0);

                    int flag = 0;

                    if (conn_ptr) {

                        printf("\n Db connect OK\n");
                        memset(sql, 0, 1024);

                       // sprintf(sql, "SELECT * FROM mail WHERE mail.receiverID = %d AND mail.isRecTrash = 0 AND mail.isRecBin = 0 ORDER BY mail.mailID DESC", mail.user.id);

                        sprintf(sql, "SELECT * FROM mail WHERE mail.receiverID = %d AND isRecBin = 0 ORDER BY mail.mailID DESC",mail.user.id);


                        printf("SQL: %s\n", sql);

                        //

                        res = mysql_query(conn_ptr, sql);
                        if (!res) {     //输出受影响的行数
                            flag = 1;
                            printf("Inserted %lu rows\n",(unsigned long)mysql_affected_rows(conn_ptr));
                        }  else {       //打印出错误代码及详细信息
                            fprintf(stderr, "Insert error %d: %sn",mysql_errno(conn_ptr),mysql_error(conn_ptr));
                        }

                    } else {
                        printf("Connection failed\n");
                    }
                    res_ptr  = mysql_store_result(conn_ptr);
                    if(res_ptr) {
                        rows = (unsigned long)mysql_num_rows(res_ptr); // rows count
                        printf("======There are %lu Rows Fetched.==\n", rows);
                        list.sum = rows;
                        list.userid = mail.user.id;
                        printf("List-sum: %d\n", list.sum);

                        cols = mysql_num_fields(res_ptr);
                        printf("======There are %d Cols Fetched.\n", cols);

                        int i=0, j=0;

                        // 读SQL返回的二维表并存储
                        while((sqlrow = mysql_fetch_row(res_ptr))) {
                            for(i = 0; i < cols; i++)
                                printf("%s\t", sqlrow[i]);
                            printf("\n");
                            list.list[j].info.mailID = atoi(sqlrow[0]);
                            list.list[j].info.senderID = atoi(sqlrow[1]);
                            list.list[j].info.receiverID = atoi(sqlrow[2]);
                            list.list[j].info.isRecStar = atoi(sqlrow[3]);
                            list.list[j].info.isSendStar = atoi(sqlrow[4]);
                            list.list[j].info.isRecTrash = atoi(sqlrow[5]);
                            list.list[j].info.isSendTrash = atoi(sqlrow[6]);
                            list.list[j].info.isRecBin = atoi(sqlrow[7]);
                            list.list[j].info.isSendBin = atoi(sqlrow[8]);
                            list.list[j].info.isDraft = atoi(sqlrow[9]);
                            strncpy(list.list[j].info.title, sqlrow[10], strlen(sqlrow[10]));
                            strncpy(list.list[j].content, sqlrow[11], strlen(sqlrow[11]));
                            j++;
                        }
                        if (mysql_errno(conn_ptr)) {
                           fprintf(stderr,"Retrive error:s\n",mysql_error(conn_ptr));
                       }
                    }
                    mysql_free_result(res_ptr);
                    mysql_close(conn_ptr);
                    send(client, &list, sizeof(struct mailList), 0);
                    cout << "Query R_ALL Compleled." << endl;
                }

                /* -------------------------------------------------*
                 *                     R-BIN
                 *                     回收站
                 * -------------------------------------------------*/

                else if (strncmp(buffer, "R_BIN", 4) == 0) {
                    memset(&mail, 0, 1024);
                    memcpy(&mail, buffer, 1024);

                    printf("Client Send a Mail\n");

                    conn_ptr = mysql_init(NULL);
                    if (!conn_ptr) {
                        printf("mysql_init failed\n");
                        return EXIT_FAILURE;
                    }

                    conn_ptr = mysql_real_connect(conn_ptr, HOST, USER_NAME, PASSWD, DATABASE, PORT, NULL, 0);

                    int flag = 0;

                    if (conn_ptr) {

                        printf("\n Db connect OK\n");
                        memset(sql, 0, 1024);

                       // sprintf(sql, "SELECT * FROM mail WHERE mail.receiverID = %d AND mail.isRecTrash = 0 AND mail.isRecBin = 0 ORDER BY mail.mailID DESC", mail.user.id);
                        sprintf(sql, "SELECT mail.* FROM mail WHERE mail.receiverID = %d AND mail.isRecBin = 1 ORDER BY mail.mailID DESC",mail.user.id);


                        printf("SQL: %s\n", sql);

                        //

                        res = mysql_query(conn_ptr, sql);
                        if (!res) {     //输出受影响的行数
                            flag = 1;
                            printf("Inserted %lu rows\n",(unsigned long)mysql_affected_rows(conn_ptr));
                        }  else {       //打印出错误代码及详细信息
                            fprintf(stderr, "Insert error %d: %sn",mysql_errno(conn_ptr),mysql_error(conn_ptr));
                        }

                    } else {
                        printf("Connection failed\n");
                    }
                    res_ptr  = mysql_store_result(conn_ptr);
                    if(res_ptr) {
                        rows = (unsigned long)mysql_num_rows(res_ptr); // rows count
                        printf("======There are %lu Rows Fetched.==\n", rows);
                        list.sum = rows;
                        list.userid = mail.user.id;
                        printf("List-sum: %d\n", list.sum);

                        cols = mysql_num_fields(res_ptr);
                        printf("======There are %d Cols Fetched.\n", cols);

                        int i=0, j=0;

                        // 读SQL返回的二维表并存储
                        while((sqlrow = mysql_fetch_row(res_ptr))) {
                            for(i = 0; i < cols; i++)
                                printf("%s\t", sqlrow[i]);
                            printf("\n");
                            list.list[j].info.mailID = atoi(sqlrow[0]);
                            list.list[j].info.senderID = atoi(sqlrow[1]);
                            list.list[j].info.receiverID = atoi(sqlrow[2]);
                            list.list[j].info.isRecStar = atoi(sqlrow[3]);
                            list.list[j].info.isSendStar = atoi(sqlrow[4]);
                            list.list[j].info.isRecTrash = atoi(sqlrow[5]);
                            list.list[j].info.isSendTrash = atoi(sqlrow[6]);
                            list.list[j].info.isRecBin = atoi(sqlrow[7]);
                            list.list[j].info.isSendBin = atoi(sqlrow[8]);
                            list.list[j].info.isDraft = atoi(sqlrow[9]);
                            strncpy(list.list[j].info.title, sqlrow[10], strlen(sqlrow[10]));
                            strncpy(list.list[j].content, sqlrow[11], strlen(sqlrow[11]));
                            j++;
                        }
                        if (mysql_errno(conn_ptr)) {
                           fprintf(stderr,"Retrive error:s\n",mysql_error(conn_ptr));
                       }
                    }
                    mysql_free_result(res_ptr);
                    mysql_close(conn_ptr);
                    send(client, &list, sizeof(struct mailList), 0);
                    cout << "Query R_BIN Compleled." << endl;
                }

                /* -------------------------------------------------*
                 *                     R-STAR
                 *                     收藏夹
                 * -------------------------------------------------*/

                else if (strncmp(buffer, "R_STAR", 4) == 0) {
                    memset(&mail, 0, 1024);
                    memcpy(&mail, buffer, 1024);

                    printf("Client Request a Star\n");

                    conn_ptr = mysql_init(NULL);
                    if (!conn_ptr) {
                        printf("mysql_init failed\n");
                        return EXIT_FAILURE;
                    }

                    conn_ptr = mysql_real_connect(conn_ptr, HOST, USER_NAME, PASSWD, DATABASE, PORT, NULL, 0);

                    int flag = 0;

                    if (conn_ptr) {

                        printf("\n Db connect OK\n");
                        memset(sql, 0, 1024);

                       // sprintf(sql, "SELECT * FROM mail WHERE mail.receiverID = %d AND mail.isRecTrash = 0 AND mail.isRecBin = 0 ORDER BY mail.mailID DESC", mail.user.id);
                        sprintf(sql, "SELECT mail.* FROM mail WHERE mail.receiverID = %d AND mail.isRecStar = 1 ORDER BY mail.mailID DESC", mail.user.id);


                        printf("SQL: %s\n", sql);

                        res = mysql_query(conn_ptr, sql);
                        if (!res) {     //输出受影响的行数
                            flag = 1;
                            printf("Inserted %lu rows\n",(unsigned long)mysql_affected_rows(conn_ptr));
                        }  else {       //打印出错误代码及详细信息
                            fprintf(stderr, "Insert error %d: %sn",mysql_errno(conn_ptr),mysql_error(conn_ptr));
                        }

                    } else {
                        printf("Connection failed\n");
                    }
                    res_ptr  = mysql_store_result(conn_ptr);
                    if(res_ptr) {
                        rows = (unsigned long)mysql_num_rows(res_ptr); // rows count
                        printf("======There are %lu Rows Fetched.==\n", rows);
                        list.sum = rows;
                        list.userid = mail.user.id;
                        printf("List-sum: %d\n", list.sum);

                        cols = mysql_num_fields(res_ptr);
                        printf("======There are %d Cols Fetched.\n", cols);

                        int i=0, j=0;

                        // 读SQL返回的二维表并存储
                        while((sqlrow = mysql_fetch_row(res_ptr))) {
                            for(i = 0; i < cols; i++)
                                printf("%s\t", sqlrow[i]);
                            printf("\n");
                            list.list[j].info.mailID = atoi(sqlrow[0]);
                            list.list[j].info.senderID = atoi(sqlrow[1]);
                            list.list[j].info.receiverID = atoi(sqlrow[2]);
                            list.list[j].info.isRecStar = atoi(sqlrow[3]);
                            list.list[j].info.isSendStar = atoi(sqlrow[4]);
                            list.list[j].info.isRecTrash = atoi(sqlrow[5]);
                            list.list[j].info.isSendTrash = atoi(sqlrow[6]);
                            list.list[j].info.isRecBin = atoi(sqlrow[7]);
                            list.list[j].info.isSendBin = atoi(sqlrow[8]);
                            list.list[j].info.isDraft = atoi(sqlrow[9]);
                            strncpy(list.list[j].info.title, sqlrow[10], strlen(sqlrow[10]));
                            strncpy(list.list[j].content, sqlrow[11], strlen(sqlrow[11]));
                            j++;
                        }
                        if (mysql_errno(conn_ptr)) {
                           fprintf(stderr,"Retrive error:s\n",mysql_error(conn_ptr));
                       }
                    }
                    mysql_free_result(res_ptr);
                    mysql_close(conn_ptr);
                    send(client, &list, sizeof(struct mailList), 0);
                    cout << "Query R_STAR Compleled." << endl;
                }

                /* -------------------------------------------------*
                 *                     R-SENT
                 *                     已发送
                 * -------------------------------------------------*/
                else if (strncmp(buffer, "R_SENT", 4) == 0) {
                    memset(&mail, 0, 1024);
                    memcpy(&mail, buffer, 1024);

                    printf("Client Request Sent Box.\n");

                    conn_ptr = mysql_init(NULL);
                    if (!conn_ptr) {
                        printf("mysql_init failed\n");
                        return EXIT_FAILURE;
                    }

                    conn_ptr = mysql_real_connect(conn_ptr, HOST, USER_NAME, PASSWD, DATABASE, PORT, NULL, 0);

                    int flag = 0;

                    if (conn_ptr) {

                        printf("\n Db connect OK\n");
                        memset(sql, 0, 1024);


                        sprintf(sql, "SELECT mail.* FROM mail WHERE mail.senderID = %d AND mail.isDraft = 0 AND mail.isSendBin = 0 ORDER BY mail.mailID DESC", mail.user.id);


                        printf("SQL: %s\n", sql);

                        res = mysql_query(conn_ptr, sql);
                        if (!res) {     //输出受影响的行数
                            flag = 1;
                            printf("Inserted %lu rows\n",(unsigned long)mysql_affected_rows(conn_ptr));
                        }  else {       //打印出错误代码及详细信息
                            fprintf(stderr, "Insert error %d: %sn",mysql_errno(conn_ptr),mysql_error(conn_ptr));
                        }

                    } else {
                        printf("Connection failed\n");
                    }
                    res_ptr  = mysql_store_result(conn_ptr);
                    if(res_ptr) {
                        rows = (unsigned long)mysql_num_rows(res_ptr); // rows count
                        printf("======There are %lu Rows Fetched.==\n", rows);
                        list.sum = rows;
                        list.userid = mail.user.id;
                        printf("List-sum: %d\n", list.sum);

                        cols = mysql_num_fields(res_ptr);
                        printf("======There are %d Cols Fetched.\n", cols);

                        int i=0, j=0;

                        // 读SQL返回的二维表并存储
                        while((sqlrow = mysql_fetch_row(res_ptr))) {
                            for(i = 0; i < cols; i++)
                                printf("%s\t", sqlrow[i]);
                            printf("\n");
                            list.list[j].info.mailID = atoi(sqlrow[0]);
                            list.list[j].info.senderID = atoi(sqlrow[1]);
                            list.list[j].info.receiverID = atoi(sqlrow[2]);
                            list.list[j].info.isRecStar = atoi(sqlrow[3]);
                            list.list[j].info.isSendStar = atoi(sqlrow[4]);
                            list.list[j].info.isRecTrash = atoi(sqlrow[5]);
                            list.list[j].info.isSendTrash = atoi(sqlrow[6]);
                            list.list[j].info.isRecBin = atoi(sqlrow[7]);
                            list.list[j].info.isSendBin = atoi(sqlrow[8]);
                            list.list[j].info.isDraft = atoi(sqlrow[9]);
                            strncpy(list.list[j].info.title, sqlrow[10], strlen(sqlrow[10]));
                            strncpy(list.list[j].content, sqlrow[11], strlen(sqlrow[11]));
                            j++;
                        }
                        if (mysql_errno(conn_ptr)) {
                           fprintf(stderr,"Retrive error:s\n",mysql_error(conn_ptr));
                       }
                    }
                    mysql_free_result(res_ptr);
                    mysql_close(conn_ptr);
                    send(client, &list, sizeof(struct mailList), 0);
                    cout << "Query R_SENT Compleled." << endl;
                }

                /* -------------------------------------------------*
                 *                     R-DRAFT
                 *                     草稿箱
                 * -------------------------------------------------*/
                else if (strncmp(buffer, "R_DRAFT", 4) == 0) {
                    memset(&mail, 0, 1024);
                    memcpy(&mail, buffer, 1024);

                    printf("Client Request Sent Box.\n");

                    conn_ptr = mysql_init(NULL);
                    if (!conn_ptr) {
                        printf("mysql_init failed\n");
                        return EXIT_FAILURE;
                    }

                    conn_ptr = mysql_real_connect(conn_ptr, HOST, USER_NAME, PASSWD, DATABASE, PORT, NULL, 0);

                    int flag = 0;

                    if (conn_ptr) {

                        printf("\n Db connect OK\n");
                        memset(sql, 0, 1024);


                        sprintf(sql, "SELECT mail.* FROM mail WHERE mail.senderID = %d AND mail.isDraft = 1 ORDER BY mail.mailID DESC", mail.user.id);


                        printf("SQL: %s\n", sql);

                        res = mysql_query(conn_ptr, sql);
                        if (!res) {     //输出受影响的行数
                            flag = 1;
                            printf("Inserted %lu rows\n",(unsigned long)mysql_affected_rows(conn_ptr));
                        }  else {       //打印出错误代码及详细信息
                            fprintf(stderr, "Insert error %d: %sn",mysql_errno(conn_ptr),mysql_error(conn_ptr));
                        }

                    } else {
                        printf("Connection failed\n");
                    }
                    res_ptr  = mysql_store_result(conn_ptr);
                    if(res_ptr) {
                        rows = (unsigned long)mysql_num_rows(res_ptr); // rows count
                        printf("======There are %lu Rows Fetched.==\n", rows);
                        list.sum = rows;
                        list.userid = mail.user.id;
                        printf("List-sum: %d\n", list.sum);

                        cols = mysql_num_fields(res_ptr);
                        printf("======There are %d Cols Fetched.\n", cols);

                        int i=0, j=0;

                        while((sqlrow = mysql_fetch_row(res_ptr))) {
                            for(i = 0; i < cols; i++)
                                printf("%s\t", sqlrow[i]);
                            printf("\n");
                            list.list[j].info.mailID = atoi(sqlrow[0]);
                            list.list[j].info.senderID = atoi(sqlrow[1]);
                            list.list[j].info.receiverID = atoi(sqlrow[2]);
                            list.list[j].info.isRecStar = atoi(sqlrow[3]);
                            list.list[j].info.isSendStar = atoi(sqlrow[4]);
                            list.list[j].info.isRecTrash = atoi(sqlrow[5]);
                            list.list[j].info.isSendTrash = atoi(sqlrow[6]);
                            list.list[j].info.isRecBin = atoi(sqlrow[7]);
                            list.list[j].info.isSendBin = atoi(sqlrow[8]);
                            list.list[j].info.isDraft = atoi(sqlrow[9]);
                            strncpy(list.list[j].info.title, sqlrow[10], strlen(sqlrow[10]));
                            strncpy(list.list[j].content, sqlrow[11], strlen(sqlrow[11]));
                            j++;
                        }
                        if (mysql_errno(conn_ptr)) {
                           fprintf(stderr,"Retrive error:s\n",mysql_error(conn_ptr));
                       }
                    }
                    mysql_free_result(res_ptr);
                    mysql_close(conn_ptr);
                    send(client, &list, sizeof(struct mailList), 0);
                    cout << "Query R_DRAFT Compleled." << endl;
                }

                /* -------------------------------------------------*
                 *                       BIN
                 *                     加回收站
                 * -------------------------------------------------*/
                else if (strncmp(buffer, "BIN", 4) == 0) {
                    memset(&mail, 0, 1024);
                    memcpy(&mail, buffer, 1024);

                    printf("Client Move a Mail to Bin.\n");

                    conn_ptr = mysql_init(NULL);
                    if (!conn_ptr) {
                        printf("mysql_init failed\n");
                        return EXIT_FAILURE;
                    }

                    conn_ptr = mysql_real_connect(conn_ptr, HOST, USER_NAME, PASSWD, DATABASE, PORT, NULL, 0);

                    int flag = 0;

                    if (conn_ptr) {

                        printf("\n Db connect OK\n");
                        memset(sql, 0, 1024);

                        sprintf(sql, "UPDATE mail SET isRecBin = 1 WHERE title = '%s';", mail.info.title);

                        printf("SQL: %s\n", sql);

                        res = mysql_query(conn_ptr, sql);
                        if (!res) {     //输出受影响的行数
                            flag = 1;
                            printf("Inserted %lu rows\n",(unsigned long)mysql_affected_rows(conn_ptr));
                        }  else {       //打印出错误代码及详细信息
                            fprintf(stderr, "Insert error %d: %sn",mysql_errno(conn_ptr),mysql_error(conn_ptr));
                        }

                    } else {
                        printf("Connection failed\n");
                    }
                    mysql_close(conn_ptr);
                    cout << "Moved to Bin! flag:" << flag << endl;
                    send(client, &flag , sizeof(int), 0);
                }

                /* -------------------------------------------------*
                 *                      STAR
                 *                     加收藏
                 * -------------------------------------------------*/

                else if (strncmp(buffer, "STAR", 4) == 0) {
                    memset(&mail, 0, 1024);
                    memcpy(&mail, buffer, 1024);

                    printf("Client Move a Mail to Bin.\n");

                    conn_ptr = mysql_init(NULL);
                    if (!conn_ptr) {
                        printf("mysql_init failed\n");
                        return EXIT_FAILURE;
                    }

                    conn_ptr = mysql_real_connect(conn_ptr, HOST, USER_NAME, PASSWD, DATABASE, PORT, NULL, 0);

                    int flag = 0;

                    if (conn_ptr) {

                        printf("\n Db connect OK\n");
                        memset(sql, 0, 1024);

                        sprintf(sql, "UPDATE mail SET isRecStar = 1 WHERE title = '%s';", mail.info.title);

                        printf("SQL: %s\n", sql);

                        res = mysql_query(conn_ptr, sql);
                        if (!res) {     //输出受影响的行数
                            flag = 1;
                            printf("Inserted %lu rows\n",(unsigned long)mysql_affected_rows(conn_ptr));
                        }  else {       //打印出错误代码及详细信息
                            fprintf(stderr, "Insert error %d: %sn",mysql_errno(conn_ptr),mysql_error(conn_ptr));
                        }

                    } else {
                        printf("Connection failed\n");
                    }
                    mysql_close(conn_ptr);
                    cout << "Added to Star! flag:" << flag << endl;
                    send(client, &flag , sizeof(int), 0);
                }

                /* -------------------------------------------------*
                 *                      DRAFT
                 *                     草稿箱
                 * -------------------------------------------------*/
                else if (strncmp(buffer, "DRAFT", 4) == 0) {
                    memset(&mail, 0, 1024);
                    memcpy(&mail, buffer, 1024);

                    printf("Client add a draft\n");

                    conn_ptr = mysql_init(NULL);
                    if (!conn_ptr) {
                        printf("mysql_init failed\n");
                        return EXIT_FAILURE;
                    }

                    conn_ptr = mysql_real_connect(conn_ptr, HOST, USER_NAME, PASSWD, DATABASE, PORT, NULL, 0);

                    int flag = 0;

                    if (conn_ptr) {

                        printf("\n Db connect OK\n");
                        memset(sql, 0, 1024);

                        mail.info.isDraft = 1;

                        sprintf(sql, "insert into mail (senderID, receiverID, isRecStar, isSendStar, "
                                     "isRecTrash, isSendTrash, isRecBin, isSendBin, isDraft, title, content) "
                                     "values (%d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', '%s');", mail.info.senderID, mail.info.receiverID,
                                     mail.info.isRecStar, mail.info.isRecTrash,
                                     mail.info.isRecTrash, mail.info.isSendTrash, mail.info.isRecBin, mail.info.isSendBin,
                                     mail.info.isDraft, mail.info.title, mail.content);

                        printf("SQL: %s\n", sql);

                        res = mysql_query(conn_ptr, sql);
                        if (!res) {     //输出受影响的行数
                            flag = 1;
                            printf("Inserted %lu rows\n",(unsigned long)mysql_affected_rows(conn_ptr));
                        }  else {       //打印出错误代码及详细信息
                            fprintf(stderr, "Insert error %d: %sn",mysql_errno(conn_ptr),mysql_error(conn_ptr));
                        }

                    } else {
                        printf("Connection failed\n");
                    }
                    mysql_close(conn_ptr);
                    cout << "Mail added to draft! flag:" << flag << endl;
                    send(client, &flag , sizeof(int), 0);
                }
                /* -------------------------------------------------*
                 *                      SEARCH
                 *                      搜索邮件
                 * -------------------------------------------------*/

                else if (strncmp(buffer, "SEARCH", 4) == 0) {
                    memset(&mail, 0, 1024);
                    memcpy(&mail, buffer, 1024);

                    printf("Client Request Search.\n");

                    conn_ptr = mysql_init(NULL);
                    if (!conn_ptr) {
                        printf("mysql_init failed\n");
                        return EXIT_FAILURE;
                    }

                    conn_ptr = mysql_real_connect(conn_ptr, HOST, USER_NAME, PASSWD, DATABASE, PORT, NULL, 0);

                    int flag = 0;

                    if (conn_ptr) {

                        printf("\n Db connect OK\n");
                        memset(sql, 0, 1024);


                        sprintf(sql, "SELECT mail.* FROM mail WHERE mail.title = '%s' AND mail.isRecBin = 0 ORDER BY mail.mailID DESC", mail.info.title);


                        printf("SQL: %s\n", sql);

                        res = mysql_query(conn_ptr, sql);
                        if (!res) {     //输出受影响的行数
                            flag = 1;
                            printf("Inserted %lu rows\n",(unsigned long)mysql_affected_rows(conn_ptr));
                        }  else {       //打印出错误代码及详细信息
                            fprintf(stderr, "Insert error %d: %sn",mysql_errno(conn_ptr),mysql_error(conn_ptr));
                        }

                    } else {
                        printf("Connection failed\n");
                    }
                    res_ptr  = mysql_store_result(conn_ptr);
                    if(res_ptr) {
                        rows = (unsigned long)mysql_num_rows(res_ptr); // rows count
                        printf("======There are %lu Rows Fetched.==\n", rows);
                        list.sum = rows;
                        list.userid = mail.user.id;
                        printf("List-sum: %d\n", list.sum);

                        cols = mysql_num_fields(res_ptr);
                        printf("======There are %d Cols Fetched.\n", cols);

                        int i=0, j=0;

                        // 读SQL返回的二维表并存储
                        while((sqlrow = mysql_fetch_row(res_ptr))) {
                            for(i = 0; i < cols; i++)
                                printf("%s\t", sqlrow[i]);
                            printf("\n");
                            list.list[j].info.mailID = atoi(sqlrow[0]);
                            list.list[j].info.senderID = atoi(sqlrow[1]);
                            list.list[j].info.receiverID = atoi(sqlrow[2]);
                            list.list[j].info.isRecStar = atoi(sqlrow[3]);
                            list.list[j].info.isSendStar = atoi(sqlrow[4]);
                            list.list[j].info.isRecTrash = atoi(sqlrow[5]);
                            list.list[j].info.isSendTrash = atoi(sqlrow[6]);
                            list.list[j].info.isRecBin = atoi(sqlrow[7]);
                            list.list[j].info.isSendBin = atoi(sqlrow[8]);
                            list.list[j].info.isDraft = atoi(sqlrow[9]);
                            strncpy(list.list[j].info.title, sqlrow[10], strlen(sqlrow[10]));
                            strncpy(list.list[j].content, sqlrow[11], strlen(sqlrow[11]));
                            j++;
                        }
                        if (mysql_errno(conn_ptr)) {
                           fprintf(stderr,"Retrive error:s\n",mysql_error(conn_ptr));
                       }
                    }
                    mysql_free_result(res_ptr);
                    mysql_close(conn_ptr);
                    send(client, &list, sizeof(struct mailList), 0);
                    cout << "Query SEARCH Compleled." << endl;
                }

                memset(buffer, 0, 1024);
            }
        }
    }
    return 0;
}

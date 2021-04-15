#ifndef CLASS_H
#define CLASS_H

// 20 bytes
struct userInfo{
    int  id;            // user id
    char password[16];  // password
};

// 20 bytes
struct socketCtrl{
    char operation[8];  // operation
    int  userid;        // uid of client
    char option[4];
    int  id;
};

// 92 bytes
struct mailInfo{
    int mailID;         // mail id
    int senderID;       // sender id
    int receiverID;     // receiver id
    int isRecStar;      // receiver control status
    int isSendStar;     // sender control status
    int isRecTrash;
    int isSendTrash;
    int isSendBin;
    int isRecBin;
    int isDraft;    // is draft or not
    char title[64];     //  mail title
};

// 1024 bytes
struct mail{
    struct socketCtrl controll; //  20 bytes
    struct userInfo user;
    struct mailInfo info;       //  92 bytes
    char   content[880];          // 976 bytes
};

struct mailList{
    int userid;
    int sum;
    struct mail list[50];
};

#endif // CLASS_H

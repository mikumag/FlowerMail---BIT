#ifndef CLASSES_H
#define CLASSES_H

// 20 bytes
struct userInfo{         // 用户信息
    int  id;             // 用户ID
    char password[16];   // 用户密码
};

// 20 bytes
struct socketCtrl{       // socket操作信息
    char operation[8];   // 操作符
    int  userid;         // 客户ID
    char option[4];      // 操作选项
    int  id;             // 操作编号
};

// 92 bytes
struct mailInfo{
    int mailID;          // 邮件唯一标识符
    int senderID;        // 发件人ID
    int receiverID;      // 收件人ID
    int isRecStar;       // 收件方加星状态
    int isSendStar;      //  发件方加星状态
    int isRecTrash;      // 收件方加回收站状态
    int isSendTrash;     //  发件方加回收站状态
    int isRecBin;        //  收件方删除状态
    int isSendBin;       //  发件方删除状态
    int isDraft;         // 是否是草稿
    char title[64];      //  标题
};

// 1024 bytes
struct mail{                     // 一封邮件
    struct socketCtrl controll;  // 操作信息
    struct userInfo user;        // 用户信息
    struct mailInfo info;        //  邮件信息
    char  content[880];          //  邮件内容


// 20488 bytes
struct mailList{
    int userid;
    int sum;
    struct mail list[20];
};

#endif // CLASSES_H

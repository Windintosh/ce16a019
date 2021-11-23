#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9152
#define BUFSIZE    512

// ���� �Լ� ���� ��� �� ����
void err_quit(char *msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);
}

// ���� �Լ� ���� ���
void err_display(char *msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    printf("[%s] %s", msg, (char *)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char *buf, int len, int flags)
{
    int received;
    char *ptr = buf;
    int left = len;

    while (left > 0) {
        received = recv(s, ptr, left, flags);
        if (received == SOCKET_ERROR)
            return SOCKET_ERROR;
        else if (received == 0)
            break;
        left -= received;
        ptr += received;
    }

    return (len - left);
}

int main(int argc, char *argv[])
{
    int retval;

    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // socket()
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) err_quit("socket()");

    // connect()
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("connect()");

    // ������ ��ſ� ����� ����
    char buf[BUFSIZE + 1];
    int len;

    FILE* fp; ///
    char fb[BUFSIZE + 1] = {0};

    // ������ ������ ���
    while (1) {
        // ������ �Է�
        printf("\n[���� ������] ");
        if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
            break;

        // '\n' ���� ����
        len = strlen(buf);
        if (buf[len - 1] == '\n')
            buf[len - 1] = '\0';
        if (strlen(buf) == 0)
            break;

        // ������ ������
        retval = send(sock, buf, strlen(buf), 0);
        if (retval == SOCKET_ERROR) {
            err_display("send()");
            break;
        } else if (buf[0] == '!' && buf[1] == 'd') { //download
            fb[0] = buf[0];
            fb[1] = buf[1];
            fp = fopen("logcpy.txt", "a+");
            printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ ���� �ٿ�ε� ��û\n", retval);
            if (!fp) {
                printf("���Ͽ��� ���� \n");
                WSACleanup();
                return 0;
            }
            else printf("���Ͽ��� ����\n");
        } else if (buf[0] == '!' && buf[1] == 'v') { //view
            fb[0] = buf[0];
            fb[1] = buf[1];
            printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ ���� ���� ��û\n", retval);
        } else if (buf[0] == '!' && buf[1] == 'o') { //overwrite
            fb[0] = buf[0];
            fb[1] = buf[1];
            printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ ����� ��û\n", retval);
        }
        else if (buf[0] == '!' && buf[1] == 'h' || buf[0] == '!' && buf[1] == '?') { //����
            printf("[TCP Ŭ���̾�Ʈ] ����\n");
            printf("1. ���� �Է��ϸ� �ش� ������ ���� �� �ؽ�Ʈ ����(log.txt)�� ����˴ϴ�.\n");
            printf("2. �ش� ������ !d ��ɾ�� �ٿ�ε尡 �����մϴ�. logcpy.txt ���Ϸ� ����˴ϴ�.\n");
            printf("3. �ش� ������ ������ !v�� Ȯ���Ͻ� �� �ֽ��ϴ�. ���Ͽ� ����� ������ ���� ��� ���� �޽����� �����մϴ�.\n");
            printf("4. !o�� ���� ���� ����Ⱑ �����մϴ�. �ڿ� �ƹ� �͵� �Է����� ������ ��� ������ �����մϴ�. \n");
            printf("5. !x�� ���� logcpy.txt ���� ������ ������ �� �ֽ��ϴ�.\n");
            printf("�ƹ� �͵� �Է����� ������ ���α׷��� �����մϴ�.\n");
            printf("!x�� ������ ��� ��ɾ�� �ڿ� ���ڿ��� �Է��ؾ� �մϴ�. �ش� ����Ʈ �� ��ŭ�� �����͸� ������մϴ�.\n");
            printf("���� ������ ��ɾ� �Է��� �ؽ�Ʈ ���Ͽ� ������� �ʽ��ϴ�.\n");
            printf("!h �Ǵ� !? ��ɾ ���� �� ������ �ٽ� �� �� �ֽ��ϴ�.\n");
        }else if (buf[0] == '!' && buf[1] == 'x') { //delete
            fb[0] = buf[0];
            fb[1] = buf[1];
            fp = fopen("logcpy.txt", "w");
            printf("[TCP Ŭ���̾�Ʈ] ����� ���� ���� ����\n");
            if (!fp) {
                printf("���Ͽ��� ���� \n");
                WSACleanup();
                return 0;
            }
            else printf("���Ͽ��� ����\n");
        }
        printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);

        // ������ �ޱ�
        retval = recvn(sock, buf, retval, 0);
        if (retval == SOCKET_ERROR) {
            err_display("recv()");
            break;
        }
        else if (retval == 0)
            break;

        // ���� ������ ���
        buf[retval] = '\0';
        if (fb[0] == '!' && fb[1] == 'd') { //�ٿ�ε�
            fp = fopen("logcpy.txt", "w");
            if (!fp) {
                printf("���Ͽ��� ���� \n");
                WSACleanup();
                return 0;
            } 
            else {
                printf("���Ͽ��� ����\n");
                fputs(buf, fp); //���Ͽ� �Է�
                printf("[TCP Ŭ���̾�Ʈ] �ٿ�ε� �Ϸ�\n");
                fclose(fp);
            }
        } else if (fb[0] == '!' && fb[1] == 'v') { //���� ����
            printf("���� ����\n");
        }
        else if (fb[0] == '!' && fb[1] == 'o') { //�����
            printf("�����\n");
        }
        else if (fb[0] == '!' && fb[1] == 'x') { //delete
            fp = fopen("logcpy.txt", "w");
            if (!fp) {
                printf("���Ͽ��� ���� \n");
                WSACleanup();
                return 0;
            }
            else {
                printf("���Ͽ��� ����\n");
                printf("[TCP Ŭ���̾�Ʈ] ���� �Ϸ�\n");
                fclose(fp);
            }
        }
        printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", retval);
        printf("[���� ������] %s\n", buf);
    }

    // closesocket()
    closesocket(sock);

    // ���� ����
    WSACleanup();
    return 0;
}

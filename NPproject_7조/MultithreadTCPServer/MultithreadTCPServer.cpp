#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

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

// Ŭ���̾�Ʈ�� ������ ���
DWORD WINAPI ProcessClient(LPVOID arg)
{
    SOCKET client_sock = (SOCKET)arg;
    int retval;
    SOCKADDR_IN clientaddr;
    int addrlen;
    char buf[BUFSIZE + 1];
    char ch[BUFSIZE + 1];

    CRITICAL_SECTION cs;

    InitializeCriticalSection(&cs); //Critical Section �ʱ�ȭ

    // Ŭ���̾�Ʈ ���� ���
    addrlen = sizeof(clientaddr);
    getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);

    FILE* fp; ///
    
    fp = fopen("log.txt", "a+");
    if (!fp) {
        printf("���� ���� ���� \n");
        WSACleanup();
        return 0;
    }
    else printf("���� ���� ����\n");

    while (1) {
        // ������ �ޱ�
        retval = recv(client_sock, buf, BUFSIZE, 0);
        if (retval == SOCKET_ERROR) {
            err_display("recv()");
            break;
        }
        else if (retval == 0) {
            break;
        }
        // ���� ������ ���
        buf[retval] = '\0';
        if (buf[0] == '!' && buf[1] == 'd') { //download
            printf("[TCP ����] �ٿ�ε� ��û\n");
            EnterCriticalSection(&cs);
            fclose(fp);
            fp = fopen("log.txt", "r");
            fgets(buf, retval-1, fp); //��ɾ �����ϰ� �Է�
            fclose(fp);
            fp = fopen("log.txt", "a+");
            LeaveCriticalSection(&cs);
        } else if (buf[0] == '!' && buf[1] == 'v') { //view
            printf("[TCP ����] ���� ���� ��û\n");
            EnterCriticalSection(&cs);
            fclose(fp);
            fp = fopen("log.txt", "r");
            fgets(buf, retval-1, fp); //��ɾ ������ ����� ��ȯ
            fclose(fp);
            fp = fopen("log.txt", "a+");
            LeaveCriticalSection(&cs);
        }  else if (buf[0] == '!' && buf[1] == 'o') { //overwrite
            printf("[TCP ����] ����� ��û\n");
            EnterCriticalSection(&cs);
            fclose(fp);
            fp = fopen("log.txt", "w"); //�� ���Ϸ� ����� ����
            strcpy(ch, buf + 2); //��ɾ�(!o)�� �����ϰ� �Է��ϱ� ����
            fputs(ch, fp);
            fclose(fp);
            fp = fopen("log.txt", "a+");
            LeaveCriticalSection(&cs);
        }
        else if (buf[0] == '!' && buf[1] == 'h' || buf[0] == '!' && buf[1] == '?') { //help
            printf("[TCP����] ��ɾ� ���� ��û\n");
        }
        else if (buf[0] == '!' && buf[1] == 'x') {
            printf("[TCP����] ���� ���� ����\n");
        }
        else { //��
            EnterCriticalSection(&cs);
            fputs(buf, fp); //���Ͽ� �Է�
            fclose(fp);
            fp = fopen("log.txt", "a+");
            LeaveCriticalSection(&cs);
        }
        printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
            ntohs(clientaddr.sin_port), buf);

        // ������ ������
        retval = send(client_sock, buf, retval, 0); //retval
        if (retval == SOCKET_ERROR) {
            err_display("send()");
            break;
        }
    }

    // closesocket()
    closesocket(client_sock);
    printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
        inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
    return 0;
}

int main(int argc, char *argv[])
{
    int retval;

    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // socket()
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("socket()");

    // bind()
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");

    // listen()
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");

    // ������ ��ſ� ����� ����
    SOCKET client_sock;
    SOCKADDR_IN clientaddr;
    int addrlen;
    HANDLE hThread;

    while (1) {
        // accept()
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            err_display("accept()");
            break;
        }

        // ������ Ŭ���̾�Ʈ ���� ���
        printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
            inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        // ������ ����
        hThread = CreateThread(NULL, 0, ProcessClient,
            (LPVOID)client_sock, 0, NULL);
        if (hThread == NULL) { closesocket(client_sock); }
        else { CloseHandle(hThread); }
    }

    // closesocket()
    closesocket(listen_sock);

    // ���� ����
    WSACleanup();
    return 0;
}
